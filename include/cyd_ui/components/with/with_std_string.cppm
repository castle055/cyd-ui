// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

export module cydui.components:with_std_string;

import std;

export import :holder;

export {
  template <>
  struct with<std::string>: public cyd::ui::components::with_data_t<std::string> {
    with& match(const std::unordered_map<std::string, cyd::ui::components::component_builder_t>& map) {
      if ((not this->val.empty()) && map.contains(this->val)) {
        auto& cs = map.at(this->val);
        std::string id;
        id = ":match(";
        id.append(this->val);
        id = ")";

        for (const auto& component_pair: cs.get_component_constructors()) {
          auto [_id, component] = component_pair;
          // make copy of id for modification
          std::string final_id  = id;
          final_id.append(":");
          final_id.append(_id);
          this->selection.emplace_back(final_id, component);
        }
      }
      return *this;
    }
    with& match(const std::vector<std::pair<std::string, cyd::ui::components::component_builder_t>>& map) {
      if ((not this->val.empty())) {
        for (const auto& [str, builder]: map) {
          if (str != this->val) {
            continue;
          }

          auto&       cs = builder;
          std::string id;
          id = ":match(";
          id.append(this->val);
          id = ")";

          for (const auto& component_pair: cs.get_component_constructors()) {
            auto [_id, component] = component_pair;
            // make copy of id for modification
            std::string final_id  = id;
            final_id.append(":");
            final_id.append(_id);
            this->selection.emplace_back(final_id, component);
          }
          break;
        }
      }
      return *this;
    }
  };
}
