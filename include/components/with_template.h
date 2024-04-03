//
// Created by castle on 10/26/23.
//

#ifndef CYD_UI_WITH_TEMPLATE_H
#define CYD_UI_WITH_TEMPLATE_H

#include <unordered_map>
#include <string>
#include <memory>

#include "component_types.h"

namespace cydui::components::with {
    template<typename T>
    struct with_data_t {
      T &val;
      
      with_data_t(T &val): val(val) { }
      with_data_t(T &&val): val(val) { }
      
      const auto &get_selection() const {
        return selection;
      }
    protected:
      std::vector<std::pair<std::string, std::function<component_base_t*()>>> selection {};
    };
    
    template<typename T>
    struct with: public with_data_t<T> {
      explicit with(T &val): with_data_t<T>(val) { }
      explicit with(T &&val): with_data_t<T>(val) { }
    };
}

#endif //CYD_UI_WITH_TEMPLATE_H
