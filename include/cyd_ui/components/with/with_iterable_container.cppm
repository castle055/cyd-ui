// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

export module cydui.components:with_iterable_container;

import std;

export import :holder;

export {
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
}
