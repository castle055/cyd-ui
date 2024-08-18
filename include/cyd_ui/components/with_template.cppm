// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

export module cydui.components:with_template;

export import :type;

export namespace cyd::ui::components::with {
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
