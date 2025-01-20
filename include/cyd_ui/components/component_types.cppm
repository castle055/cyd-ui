// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

export module cydui.components:type;

import std;

export import reflect;
export import quantify;

export import cydui.dimensions;

export namespace cyd::ui::components {
  using dimension_t = dimensions::dimension<dimensions::screen_measure>;
  using dimension_parameter_t = dimensions::parameter<dimensions::screen_measure>;
  using dimension_ctx_t = dimensions::context<dimensions::screen_measure>;

  struct component_state_t;
  using component_state_ref = std::shared_ptr<component_state_t>;
  struct component_base_t;
  struct event_handler_t;

  template <typename EVH>
  concept ComponentEventHandlerConcept =
    requires(EVH evh) { std::is_base_of_v<event_handler_t, EVH>; };

  template <typename T>
  struct component_t;

  template <typename C>
  concept ComponentConcept = requires(C c) {
    requires std::
      derived_from<std::remove_reference_t<C>, component_t<C>>;
    typename C::props_t;
    typename C::state_t;
    typename C::event_handler_t;
    { C::NAME } -> std::convertible_to<std::string>;
  };

  struct internal_relations_t {
    dimension_t cx{};
    dimension_t cy{};
    dimension_t cw{};
    dimension_t ch{};
  };

  struct component_dimensional_relations_t {
    dimension_t& x;
    dimension_t& y;
    dimension_t& w;
    dimension_t& h;
    bool&        fixed_w;
    bool&        fixed_h;

    dimension_t& cx;
    dimension_t& cy;
    dimension_t& cw;
    dimension_t& ch;

    dimension_t& margin_top;
    dimension_t& margin_bottom;
    dimension_t& margin_left;
    dimension_t& margin_right;

    dimension_t& padding_top;
    dimension_t& padding_bottom;
    dimension_t& padding_left;
    dimension_t& padding_right;
  };
} // namespace cyd::ui::components
