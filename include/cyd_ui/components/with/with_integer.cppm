// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

export module cydui.components:with_integer;

import std;

export import :holder;

import :with_iterable_container;

export {
  template <>
  struct with<int>: public cyd::ui::components::with_data_t<int> {
    with<int>& eq() {
      return *this;
    }

    with<int>& for_each(const std::function<map_to_result_t(int& value)>& transform) {
      std::string id;
      std::size_t j = 0;
      std::string index_suffix;
      std::string jndex_suffix;
      for (int i = 0; i < val; ++i) {
        index_suffix = ":";
        index_suffix.append(std::to_string(i));
        auto cs = transform(i);
        for (const auto& item1: cs.result) {
          jndex_suffix = ":";
          jndex_suffix.append(std::to_string(j));

          for (const auto& component_pair: item1.get_component_constructors()) {
            auto [_, component] = component_pair;
            // make copy of id for modification
            id                  = component_pair.first;
            id.append(jndex_suffix);
            if (cs.id.empty()) {
              id.append(index_suffix);
            } else {
              id.append(":");
              id.append(cs.id);
            }
            id.append(":map_to");
            this->selection.emplace_back(id, component);
          }
          ++j;
        }
      }
      return *this;
    }
  };
}
