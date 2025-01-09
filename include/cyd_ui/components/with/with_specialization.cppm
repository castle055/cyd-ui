// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

export module cydui.components:with_specialization;

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

  template <typename I>
  concept IterableContainer = requires(I i) {
    typename I::iterator;
    typename I::value_type;
    { i.begin() } -> std::same_as<typename I::iterator>;
    { i.end() } -> std::same_as<typename I::iterator>;
    { i.size() } -> std::same_as<std::size_t>;
    // { std::begin(i) } -> std::same_as<typename I::iterator>;
    // { std::end(i) } -> std::same_as<typename I::iterator>;
    // { std::size(i) } -> std::same_as<std::size_t>;
  };

  struct map_to_result_t {
    std::string                      id;
    std::vector<cyd::ui::components::component_builder_t> result;

    map_to_result_t(std::initializer_list<cyd::ui::components::component_builder_t> result)
        : result(result) {}
  };

  template <IterableContainer I>
  struct with<I>: public cyd::ui::components::with_data_t<I> {
    with<I>& map_to(
      std::function<map_to_result_t(std::size_t index, typename I::value_type& value)> transform
    ) {
      std::size_t i = 0;
      for (auto item = std::begin(this->val); item != std::end(this->val); ++item) {
        std::size_t j = 0;
        auto cs       = transform(i, *item);
        for (const auto& item1: cs.result) {
          std::size_t k = 0;
          for (const auto& component_pair: item1.get_component_constructors()) {
            auto [id, component] = component_pair;
            std::string full_id = std::format(":map_to:{}:{}:{}", i, j, k);
            if (not cs.id.empty()) {
              full_id.append(":");
              full_id.append(cs.id);
            }
            this->selection.emplace_back(full_id.append(id), component);
            ++k;
          }
          ++j;
        }
        ++i;
      }
      return *this;
    }
    with<I>& map_to(std::function<map_to_result_t(typename I::value_type& value)> transform) {
      return map_to([=](auto i, auto v) { return transform(v); });
    }
  };

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
