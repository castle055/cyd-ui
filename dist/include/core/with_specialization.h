//
// Created by castle on 10/26/23.
//

#ifndef CYD_UI_WITH_SPECIALIZATION_H
#define CYD_UI_WITH_SPECIALIZATION_H

#include "component_holder.h"

#include <functional>

namespace cydui::components::with {
    
    template<>
    struct with<bool>: public with_data_t<bool> {
      with<bool> &then(std::function<std::vector<component_holder_t>()> components) {
        if (val) {
          selection.clear();
          auto cs = components();
          std::size_t i = 0;
          std::string index_suffix;
          std::string id;
          for (const auto &item: cs) {
            index_suffix = ":";
            index_suffix.append(std::to_string(i));
            
            for (const auto &component_pair: item.get_components()) {
              auto [_, component] = component_pair;
              // make copy of id for modification
              id = component_pair.first;
              id.append(index_suffix);
              id.append(":then");
              selection[id] = component;
            }
            
            ++i;
          }
        }
        return *this;
      }
//#define then(...) \
//  _Pragma("clang diagnostic push") \
//  _Pragma("clang diagnostic ignored \"-Wunused-lambda-capture\"") \
//  then([this]{ return std::vector<cydui::components::component_holder_t> __VA_ARGS__ ; }) \
//  _Pragma("clang diagnostic pop")
      
      with<bool> &or_else(std::function<std::vector<component_holder_t>()> components) {
        if (!val) {
          selection.clear();
          auto cs = components();
          std::size_t i = 0;
          std::string index_suffix;
          std::string id;
          for (const auto &item: cs) {
            index_suffix = ":";
            index_suffix.append(std::to_string(i));
            
            for (const auto &component_pair: item.get_components()) {
              auto [_, component] = component_pair;
              // make copy of id for modification
              id = component_pair.first;
              id.append(index_suffix);
              id.append(":or_else");
              selection[id] = component;
            }
            
            ++i;
          }
        }
        return *this;
      }
#define or_else(...) \
  _Pragma("clang diagnostic push") \
  _Pragma("clang diagnostic ignored \"-Wunused-lambda-capture\"") \
  or_else([this]{ return std::vector<component_holder_t> __VA_ARGS__ ; }) \
  _Pragma("clang diagnostic pop")
    };
    
    template<typename I>
    concept IterableContainer = requires(I i) {
      typename I::iterator;
      typename I::value_type;
      {std::begin(i)} -> std::same_as<typename I::iterator>;
      {std::end(i)} -> std::same_as<typename I::iterator>;
      {std::size(i)} -> std::same_as<std::size_t>;
    };
    
    struct map_to_result_t {
      std::string id;
      std::vector<component_holder_t> result;
      
      map_to_result_t(std::initializer_list<component_holder_t> result)
        : result(result) { }
    };
    
    template<IterableContainer I>
    struct with<I>: public with_data_t<I> {
      with<I> &map_to(std::function<map_to_result_t(typename I::value_type &value)> transform) {
        std::string id;
        std::size_t i = 0;
        std::size_t j = 0;
        std::string index_suffix;
        std::string jndex_suffix;
        for (auto item = std::begin(this->val); item != std::end(this->val); ++item) {
          index_suffix = ":";
          index_suffix.append(std::to_string(i));
          auto cs = transform(*item);
          for (const auto &item1: cs.result) {
            jndex_suffix = ":";
            jndex_suffix.append(std::to_string(j));
            
            for (const auto &component_pair: item1.get_components()) {
              auto [_, component] = component_pair;
              // make copy of id for modification
              id = component_pair.first;
              id.append(jndex_suffix);
              if (cs.id.empty()) {
                id.append(index_suffix);
              } else {
                id.append(":");
                id.append(cs.id);
              }
              id.append(":map_to");
              this->selection[id] = component;
            }
            ++j;
          }
          ++i;
        }
        return *this;
      }
#define map_to(...) \
  _Pragma("clang diagnostic push") \
  _Pragma("clang diagnostic ignored \"-Wunused-lambda-capture\"") \
  map_to( __VA_ARGS__ ) \
  _Pragma("clang diagnostic pop")
    
    };
    
    template<>
    struct with<int>: public with_data_t<int> {
      with<int> &eq() {
        return *this;
      }
      
      with<int> &for_each(const std::function<map_to_result_t(int &value)> &transform) {
        std::string id;
        std::size_t j = 0;
        std::string index_suffix;
        std::string jndex_suffix;
        for (int i = 0; i < val; ++i) {
          index_suffix = ":";
          index_suffix.append(std::to_string(i));
          auto cs = transform(i);
          for (const auto &item1: cs.result) {
            jndex_suffix = ":";
            jndex_suffix.append(std::to_string(j));
            
            for (const auto &component_pair: item1.get_components()) {
              auto [_, component] = component_pair;
              // make copy of id for modification
              id = component_pair.first;
              id.append(jndex_suffix);
              if (cs.id.empty()) {
                id.append(index_suffix);
              } else {
                id.append(":");
                id.append(cs.id);
              }
              id.append(":map_to");
              this->selection[id] = component;
            }
            ++j;
          }
        }
        return *this;
      }
    };
}

#endif //CYD_UI_WITH_SPECIALIZATION_H
