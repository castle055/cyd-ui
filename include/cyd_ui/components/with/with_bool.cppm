// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

export module cydui.components:with_bool;

import std;

export import cydui.components.base;


export template <>
struct with<bool>: public cydui::components::with_data_t<bool> {
  explicit with(bool& val)
      : cydui::components::with_data_t<bool>(val) {}
  explicit with(bool&& val)
      : cydui::components::with_data_t<bool>(val) {}

  with& then(const std::vector<cydui::components::component_builder_t>& components) {
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

  with<bool>& or_else(const std::vector<cydui::components::component_builder_t>& components) {
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

