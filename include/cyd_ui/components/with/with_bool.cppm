// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

export module cydui.components:with_bool;

import std;

export import :holder;

export {
  template <>
  struct with<bool>: public cyd::ui::components::with_data_t<bool> {
    with<bool>& then(const std::vector<cyd::ui::components::component_builder_t>& components) {
      if (val) {
        selection.clear();

        std::size_t i {0};
        for (const auto &item: components) {
          std::size_t j {0};
          for (const auto &component_pair: item.get_component_constructors()) {
            auto &[id, builder] = component_pair;
            selection.emplace_back(std::format(":then:{}:{}{}", i, j, id), builder);
            ++j;
          }
          ++i;
        }
      }
      return *this;
    }

    with<bool>& or_else(const std::vector<cyd::ui::components::component_builder_t>& components) {
      if (!val) {
        selection.clear();

        std::size_t i {0};
        for (const auto& item: components) {
          std::size_t j {0};
          for (const auto& component_pair: item.get_component_constructors()) {
            auto [id, builder] = component_pair;
            selection.emplace_back(std::format(":or_else:{}:{}{}", i, j, id), builder);
            ++j;
          }
          ++i;
        }
      }
      return *this;
    }
  };
}
