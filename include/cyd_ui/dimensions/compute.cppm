/*! \file  compute.cppm
 *! \brief 
 *!
 */

export module cydui.dimensions:compute;

import std;

import fabric.logging;

import :types;
import :expression;
import :impl;


namespace cyd::ui::dimensions {
 export template <typename T>
 struct cycle_t {
  std::deque<typename dimension_impl<T>::sptr> dimensions { };
 };

 export template<typename T>
 struct compute_result_t {
  std::vector<cycle_t<T>> cycles { };

  operator bool() const {
   return cycles.empty();
  }
 };

 template <typename T>
 bool find_cycle(cycle_t<T>& cycle, typename dimension_impl<T>::sptr start, typename dimension_impl<T>::sptr head) {
  for (const auto & dependency : head->expr().dependencies()) {
   if (dependency == start) {
    cycle.dimensions.emplace_front(dependency);
    return true;
   } else {
    if (find_cycle(cycle, start, dependency)) {
     cycle.dimensions.emplace_front(dependency);
     return true;
    }
   }
  }
  return false;
 }

 template <typename T>
 bool evaluate_expression(typename dimension_impl<T>::sptr dim, const std::unordered_map<std::string, dimension<T>>& parameters) {
   using expression = expression<T>;
   expression& expr = dim->expr();

   std::deque<typename expression::node_t::sptr>                           stack{};
   std::stack<std::pair<typename expression::node_t::sptr, std::deque<T>>> result_stack{};
   stack.emplace_back(expr.tree());
   result_stack.emplace(typename expression::node_t::sptr{nullptr}, std::deque<T>{});

  //  LOG::print{FATAL
  //  }("Something went wrong while evaluating an expression, result_stack invalid: {}",
  //    expr.to_string());
   while (!stack.empty()) {
     auto top = stack.back();
     if (top == nullptr) {
       result_stack.top().second.push_back(T{});
       stack.pop_back();
       continue;
     }

     switch (top->op) {
       case expression::node_t::CONSTANT:
         result_stack.top().second.push_back(top->const_value);
         stack.pop_back();
         break;
       case expression::node_t::DIMENSION:
         result_stack.top().second.push_back(top->dimension->value_);
         stack.pop_back();
         break;
       case expression::node_t::PARAMETER:
         if (dim->context_->contains(top->parameter.name)) {
           auto d = (*dim->context_)[top->parameter.name]();
           result_stack.top().second.push_back((*dim->context_)[top->parameter.name]().value());
           stack.pop_back();
         } else if (parameters.contains(top->parameter.name)) {
           result_stack.top().second.push_back(parameters.at(top->parameter.name).value());
           stack.pop_back();
         } else {
           LOG::print{FATAL
           }("Missing parameter while evaluating expression: {}", top->parameter.name);
           return false;
         }
         break;
       case expression::node_t::ADDITION:
       case expression::node_t::SUBTRACTION:
       case expression::node_t::MULTIPLICATION:
       case expression::node_t::DIVISION:
         if (result_stack.top().first != top) {
           result_stack.emplace(top, std::deque<T>{});
           for (auto it = top->children.rbegin(); it != top->children.rend(); ++it) {
             stack.emplace_back(*it);
           }
         } else {
           auto res = result_stack.top().second;
           T    accumulator{res.front()};
           switch (top->op) {
             case expression::node_t::ADDITION:
               for (auto it = std::next(res.begin()); it != res.end(); ++it) {
                 accumulator.value += (*it).template as<screen::pixel>().value;
               }
               break;
             case expression::node_t::SUBTRACTION:
               for (auto it = std::next(res.begin()); it != res.end(); ++it) {
                 accumulator.value -= (*it).template as<screen::pixel>().value;
               }
               break;
             case expression::node_t::MULTIPLICATION:
               for (auto it = std::next(res.begin()); it != res.end(); ++it) {
                 accumulator.value *= (*it).template as<screen::pixel>().value;
               }
               break;
             case expression::node_t::DIVISION:
               for (auto it = std::next(res.begin()); it != res.end(); ++it) {
                 accumulator.value /= (*it).template as<screen::pixel>().value;
               }
               break;
             default:
               break;
           }
           result_stack.pop();
           result_stack.top().second.push_back(accumulator);
           stack.pop_back();
         }
         break;
       default:
         LOG::print{FATAL} //
         ("Expression node has invalid operator {}", static_cast<std::size_t>(top->op));
         break;
     }
   }

   if (result_stack.size() == 1 && result_stack.top().first == nullptr &&
       result_stack.top().second.size() == 1) {
     dim->value_   = result_stack.top().second.front();
     dim->unknown_ = false;
     return true;
   }
   LOG::print{FATAL
   }("Something went wrong while evaluating an expression, result_stack invalid: {}",
     expr.to_string());
   return false;
 }

 export template<typename T>
 compute_result_t<T> compute_dimension(dimension<T>& dim_, const std::unordered_map<std::string, dimension<T>>& parameters = {}) {
  typename dimension_impl<T>::sptr dim = dim_.impl();
  std::unordered_set<typename dimension_impl<T>::sptr> visited { };
  std::deque<typename dimension_impl<T>::sptr> stack { };
  std::vector<cycle_t<T>> cycles { };
  stack.emplace_back(dim);
  visited.insert(dim);

  while (!stack.empty()) {
   auto top = stack.back();
   if (top->expr().dependencies().empty()) {
    if (top->is_unknown()) {
     evaluate_expression<T>(top, parameters);
    }
    stack.pop_back();
    visited.erase(top);
   } else {
    bool work_to_do = false;

    for (auto it = top->expr().dependencies().begin(); it != top->expr().dependencies().end(); ++it) {
     auto dep = *it;
     if (!dep->is_unknown()) continue;
     if (visited.contains(dep)) {
      //! Cycle detected
      cycle_t<T> cycle { };
      LOG::print {ERROR}("Cycle detected.");
      if (!find_cycle(cycle, dep, dep)) {
       LOG::print {FATAL}("Cycle detection gave a false positive.");
      }
      cycles.emplace_back(cycle);
     } else {
      visited.insert(dep);
      stack.emplace_back(dep);
      work_to_do = true;
      }
    }

    if (!work_to_do) {
     if (top->is_unknown()) {
      evaluate_expression<T>(top, parameters);
     }
     stack.pop_back();
     visited.erase(top);
    }
   }
  }

  return {cycles};
 }

 // export template <typename T>
 // const T& get_value(const dimension<T>& dim) {
 //  //  LOG::print{INFO}("Value: {}", dim.value().to_string());
 //   return dim.value();
 // }
}
