// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

export module cydui.components:with_std_variant;

import std;

export import :holder;

export {
  template <typename... Ts>
  struct with<std::variant<Ts...>>: cyd::ui::components::with_data_t<std::variant<Ts...>> {
    template <typename T>
    with& if_type_is(const std::function<std::vector<cyd::ui::components::component_builder_t>(T& value)>& builder) {
      std::variant<Ts...> v = this->val;
      if (std::holds_alternative<T>(this->val)) {
        T&          value = std::get<T>(this->val);
        auto        cs    = builder(value);
        std::string id;
        std::size_t i = 0;
        std::string index_suffix;
        id = ":if_type_is<";
        id.append(std::string{typeid(T).name()});
        id = ">";

        for (const auto& item1: cs) {
          index_suffix = ":";
          index_suffix.append(std::to_string(i));

          for (const auto& component_pair: item1.get_component_constructors()) {
            auto [_id, component] = component_pair;
            // make copy of id for modification
            std::string final_id  = id;
            final_id.append(":");
            final_id.append(_id);
            this->selection.emplace_back(final_id, component);
          }
          ++i;
        }
      }
      return *this;
    }
    template <typename T>
    with& if_type_is(std::vector<cyd::ui::components::component_builder_t>& cs) {
      std::variant<Ts...> v = this->val;
      if (std::holds_alternative<T>(this->val)) {
        T&          value = std::get<T>(this->val);
        std::string id;
        std::size_t i = 0;
        std::string index_suffix;
        id = ":if_type_is<";
        id.append(std::string{typeid(T).name()});
        id = ">";

        for (const auto& item1: cs) {
          index_suffix = ":";
          index_suffix.append(std::to_string(i));

          for (const auto& component_pair: item1.get_component_constructors()) {
            auto [_id, component] = component_pair;
            // make copy of id for modification
            std::string final_id  = id;
            final_id.append(":");
            final_id.append(_id);
            this->selection.emplace_back(final_id, component);
          }
          ++i;
        }
      }
      return *this;
    }
    template <typename T>
    with& if_type_is(std::vector<cyd::ui::components::component_builder_t>&& cs) {
      return if_type_is<T>(cs);
    }
  };
}
