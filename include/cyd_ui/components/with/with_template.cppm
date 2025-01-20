// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

export module cydui.components:with_template;

import std;
export import :type;


export template <typename T>
struct with;

namespace cyd::ui::components {
    export template<typename T>
    struct with_data_t {
      T val;
      
      with_data_t(T &val): val(val) { }
      with_data_t(T &&val): val(val) { }
      
      const auto &get_selection() const {
        return selection;
      }
    protected:
      std::vector<std::pair<std::string, std::function<std::shared_ptr<component_base_t>()>>> selection {};
    };
} // namespace cyd::ui::components::with

export template <typename T>
struct with: public cyd::ui::components::with_data_t<T> {
  explicit with(T& val)
      : cyd::ui::components::with_data_t<T>(val) {}
  explicit with(T&& val)
      : cyd::ui::components::with_data_t<T>(val) {}
};

export template <typename T>
with(T&) -> with<T>;

export template <typename T>
with(T&&) -> with<T>;
