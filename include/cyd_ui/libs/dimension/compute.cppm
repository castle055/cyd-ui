/*! \file  compute.cppm
 *! \brief
 *!
 */

export module cydui.dimensions:compute;

import std;

import fabric.logging;

export import :types;
export import :expression;
export import :impl;


namespace cydui::dimensions {
  export template <typename T>
  struct cycle_t {
    std::deque<typename dimension_impl<T>::sptr> dimensions {};
  };

  export template <typename T>
  struct compute_result_t {
    std::vector<cycle_t<T>> cycles {};
    bool                    value_changed = false;

    operator bool() const {
      return cycles.empty();
    }
  };

  template <typename T>
  bool find_cycle(
    cycle_t<T>&                      cycle,
    typename dimension_impl<T>::sptr start,
    typename dimension_impl<T>::sptr head,
    const std::unordered_map<
      std::string,
      dimension<T>>& global_parameters) {
    std::unordered_set<std::shared_ptr<dimension_impl<T>>> deps {};
    for (const auto& dep: head->expr().dependencies()) {
      deps.insert(dep);
    }
    for (const auto& param: head->expr().parameters()) {
      if (head->context_->contains(param.name)) {
        auto& param_dim = head->context_->operator[](param.name);
        deps.insert(param_dim.impl());
      } else if (global_parameters.contains(param.name)) {
        auto& param_dim = global_parameters.at(param.name);
        deps.insert(param_dim.impl());
      } else {
        LOG::print {FATAL}("Missing expression parameter: {}", param.name);
      }
    }

    for (const auto& dependency: deps) {
      if (dependency == start) {
        cycle.dimensions.emplace_front(dependency);
        return true;
      } else {
        if (find_cycle(cycle, start, dependency, global_parameters)) {
          cycle.dimensions.emplace_front(dependency);
          return true;
        }
      }
    }
    return false;
  }

  template <typename T>
  bool evaluate_expression(
    typename dimension_impl<T>::sptr dim,
    const std::unordered_map<
      std::string,
      dimension<T>>& parameters) {
    // LOG::print{DEBUG}("Evaluating expression: {} = {}", dim->name_, dim->expr_.to_string());
    using expression = expression<T>;
    expression& expr = dim->expr();

    std::deque<typename expression::node_t::sptr>                           stack {};
    std::stack<std::pair<typename expression::node_t::sptr, std::deque<T>>> result_stack {};
    stack.emplace_back(expr.tree());
    result_stack.emplace(typename expression::node_t::sptr {nullptr}, std::deque<T> {});

    //  LOG::print{FATAL
    //  }("Something went wrong while evaluating an expression, result_stack invalid: {}",
    //    expr.to_string());
    while (!stack.empty()) {
      auto top = stack.back();
      if (top == nullptr) {
        result_stack.top().second.push_back(T {});
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
            auto& d = (*dim->context_)[top->parameter.name];
            result_stack.top().second.push_back((*dim->context_)[top->parameter.name].value());
            stack.pop_back();
          } else if (parameters.contains(top->parameter.name)) {
            result_stack.top().second.push_back(parameters.at(top->parameter.name).value());
            stack.pop_back();
          } else {
            LOG::print {FATAL}(
              "Missing parameter ({}) while evaluating expression: {}", top->parameter.name, expr.to_string());
            return false;
          }
          break;
        case expression::node_t::FUNCTION:
          result_stack.top().second.push_back(top->fun.operator()());
          stack.pop_back();
          break;
        case expression::node_t::ADDITION:
        case expression::node_t::SUBTRACTION:
        case expression::node_t::MULTIPLICATION:
        case expression::node_t::DIVISION:
          if (result_stack.top().first != top) {
            result_stack.emplace(top, std::deque<T> {});
            for (auto it = top->children.rbegin(); it != top->children.rend(); ++it) {
              stack.emplace_back(*it);
            }
          } else {
            auto res = result_stack.top().second;
            T    accumulator {res.front()};
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
              default: break;
            }
            result_stack.pop();
            result_stack.top().second.push_back(accumulator);
            stack.pop_back();
          }
          break;
        default:
          LOG::print {FATAL} //
          ("Expression node has invalid operator {}", static_cast<std::size_t>(top->op));
          break;
      }
    }

    if (result_stack.size() == 1 && result_stack.top().first == nullptr && result_stack.top().second.size() == 1) {
      dim->value_   = result_stack.top().second.front();
      dim->unknown_ = false;
      return true;
    }
    LOG::print {FATAL}(
      "Something went wrong while evaluating an expression, result_stack invalid: {}", expr.to_string());
    return false;
  }

  export template <typename T>
  compute_result_t<T> compute_dimension(
    dimension<T>& dim_,
    const std::unordered_map<
      std::string,
      dimension<T>>& parameters = {}) {
    typename dimension_impl<T>::sptr                     dim = dim_.impl();
    std::unordered_set<typename dimension_impl<T>::sptr> visited {};
    std::deque<typename dimension_impl<T>::sptr>         stack {};
    std::vector<cycle_t<T>>                              cycles {};
    bool                                                 something_changed = false;
    stack.emplace_back(dim);

    while (!stack.empty()) {
      auto top = stack.back();

      if (not top->is_unknown()) {
        stack.pop_back();
        visited.erase(top);
        continue;
      }

      something_changed = true;

      std::unordered_set<std::shared_ptr<dimension_impl<T>>> deps {};
      visited.insert(top);
      for (const auto& dep: top->expr().dependencies()) {
        deps.insert(dep);
      }
      for (const auto& param: top->expr().parameters()) {
        if (top->context_->contains(param.name)) {
          auto& param_dim = top->context_->operator[](param.name);
          deps.insert(param_dim.impl());
        } else if (parameters.contains(param.name)) {
          auto& param_dim = parameters.at(param.name);
          deps.insert(param_dim.impl());
        } else {
          LOG::print {FATAL}("Missing expression parameter: {}", param.name);
        }
      }

      if (deps.empty()) {
        evaluate_expression<T>(top, parameters);
        stack.pop_back();
        visited.erase(top);
      } else {
        bool work_to_do = false;

        for (auto it = deps.begin(); it != deps.end(); ++it) {
          auto dep = *it;
          if (not dep->is_unknown()) continue;
          if (visited.contains(dep)) {
            //! Cycle detected
            cycle_t<T>  cycle {};
            std::size_t i = 0;
            // LOG::print{INFO}("Possible cycle detected:");
            // LOG::print{INFO}("START: [0x{:X}] {}::{} = {}", (unsigned long)(dim_.impl()->get()),
            // dim_.impl()->context_->get_name(), dim_.impl()->name_,
            // dim_.impl()->expr().to_string()); for (const auto& v: visited) { LOG::print{INFO}("
            // {}: [0x{:X}] {}::{} = {}", i++, (unsigned long)(v.get()), v->context_->get_name(),
            // v->name_, v->expr().to_string());
            // }

            if (!find_cycle(cycle, dep, dep, parameters)) {
              LOG::print {FATAL}("Cycle detection gave a false positive.");
            } else {
              i = 0;
              std::stringstream ss {};
              ss << "Dependency cycle in dimensions:" << std::endl;
              for (const auto& v: cycle.dimensions) {
                std::source_location loc = v->expr().location();
                std::string          loc_str =
                  std::format("{}:{}:{} ({})", loc.file_name(), loc.line(), loc.column(), loc.function_name());
                ss << std::format("  {}: {} = {} @ {}", i++, to_string(v.get()), v->expr().to_string(), loc_str)
                   << std::endl;
              }
              LOG::print {ERROR}("{}", ss.str());
            }
            cycles.emplace_back(cycle);
          } else {
            stack.emplace_back(dep);
            work_to_do = true;
          }
        }

        if (!work_to_do) {
          evaluate_expression<T>(top, parameters);
          stack.pop_back();
          visited.erase(top);
        }
      }
    }

    return {cycles, something_changed};
  }

  // export template <typename T>
  // const T& get_value(const dimension<T>& dim) {
  //  //  LOG::print{INFO}("Value: {}", dim.value().to_string());
  //   return dim.value();
  // }
} // namespace cydui::dimensions
