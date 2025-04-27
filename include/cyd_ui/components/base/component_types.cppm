// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

export module cydui.components.base:type;

import std;

export import reflect;
export import quantify;

export import cydui.dimensions;

export namespace cydui::components {
  template<std::size_t N>
  struct str_literal_t {
    constexpr str_literal_t(const char (& str)[N]) {
      std::copy_n(str, N, value);
    }

    char value[N];
  };

  class component_state_t;
  class component_state_delegate_t;
  using component_state_ref = std::shared_ptr<component_state_t>;
  class component_base_t;
  struct component_actor_t;

  template <typename C>
  concept ComponentConcept = requires(C c) {
    requires std::
      derived_from<std::remove_reference_t<C>, component_base_t>;
    typename C::props_t;
    typename C::state_t;
    typename C::event_handler_t;
  };

  struct internal_relations_t {
    dimension_t cx{};
    dimension_t cy{};
    dimension_t cw{};
    dimension_t ch{};

    dimension_t children_total_width{};
    dimension_t children_total_height{};
  };

  struct component_dimensional_relations_t {
    dimension_t& x;
    dimension_t& y;
    dimension_t& width;
    dimension_t& height;
    // bool&        fixed_w;
    // bool&        fixed_h;

    dimension_t& margin_top;
    dimension_t& margin_bottom;
    dimension_t& margin_left;
    dimension_t& margin_right;

    dimension_t& padding_top;
    dimension_t& padding_bottom;
    dimension_t& padding_left;
    dimension_t& padding_right;

    dimension_t& scroll_x;
    dimension_t& scroll_y;
  };
} // namespace cydui::components
