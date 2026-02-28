// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include "./component_event_macros.h"

#define TO_STRING(...) #__VA_ARGS__
#define ANCHOR(PREFIX, NAME)                                                                                           \
  struct NAME {                                                                                                        \
    static constexpr dimension_parameter_t x {TO_STRING(PREFIX##_##NAME##_x)};                                         \
    static constexpr dimension_parameter_t y {TO_STRING(PREFIX##_##NAME##_y)};                                         \
  }

#define CYDUI_INTERNAL_EV_HANDLER_DECL(NAME) void on_##NAME CYDUI_INTERNAL_EV_##NAME##_ARGS

#define CYDUI_INTERNAL_EV_HANDLER_DECL_W_RET(NAME)                                                                     \
  CYDUI_INTERNAL_EV_##NAME##_RETURN on_##NAME CYDUI_INTERNAL_EV_##NAME##_ARGS

export module cydui.core.event_handler;

export import std;

export import cydui.core.Component;
export import cydui.geometry.anchors;
export import cydui.event_types;


export namespace cydui::detail {


#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedMacroInspection"

  struct event_handler_t {
  private:
    Component& component;

  public:
    static constexpr bool handles_text_input = false;

    explicit event_handler_t(Component& comp)
        : component(comp) {}

    // virtual ~event_handler_t() {}

    // * mount/dismount
    CYDUI_INTERNAL_EV_HANDLER_DECL(mount) {}
    CYDUI_INTERNAL_EV_HANDLER_DECL(dismount) {}

    CYDUI_INTERNAL_EV_HANDLER_DECL_W_RET(redraw) {
      return {$content};
    }

    CYDUI_INTERNAL_EV_HANDLER_DECL(layout) {}

#define DIMENSIONAL_ARGS $x, $y, $width, $height, $padding_top, $padding_bottom, $padding_left, $padding_right

    // ? MOUSE EVENTS
    // * button press
    CYDUI_INTERNAL_EV_HANDLER_DECL(button_press) {
      propagate = true;
    }
    // * button release
    CYDUI_INTERNAL_EV_HANDLER_DECL(button_release) {
      propagate = true;
    }
    // * mouse enter
    CYDUI_INTERNAL_EV_HANDLER_DECL(mouse_enter) {}
    // * mouse exit
    CYDUI_INTERNAL_EV_HANDLER_DECL(mouse_exit) {}
    // * mouse motion
    CYDUI_INTERNAL_EV_HANDLER_DECL(mouse_motion) {
      propagate = true;
    }
    // * mouse scroll
    CYDUI_INTERNAL_EV_HANDLER_DECL(scroll) {
      auto& style     = component.get_style();
      auto& geom      = component.get_geometry();
      bool  processed = false;
      if (style.overflow_x == overflow_e::SCROLL) {
        auto c_width = dimensions::get_value(geom.content_size[layout::X_AXIS]);
        if ($width < c_width) {
          auto new_scroll = dimensions::get_value(geom.scroll[layout::X_AXIS]) + dx;
          if (new_scroll < 0_px) new_scroll = 0_px;
          if (new_scroll > (c_width - $width)) new_scroll = c_width - $width;
          component.scroll_x(new_scroll);
          processed = true;
        }
      }
      if (style.overflow_y == overflow_e::SCROLL) {
        auto c_height = dimensions::get_value(geom.content_size[layout::Y_AXIS]);
        if ($height < c_height) {
          auto new_scroll = dimensions::get_value(geom.scroll[layout::Y_AXIS]) - dy; // vertical scroll must be flipped
          if (new_scroll < 0_px) new_scroll = 0_px;
          if (new_scroll > (c_height - $height)) new_scroll = c_height - $height;
          component.scroll_y(new_scroll);
          processed = true;
        }
      }
      if (processed) {
        component.mark_dirty();
      } else {
        // Propagate event to parent
        propagate = true;
      }
    }

    // ? KEYBOARD EVENTS
    // * key press
    CYDUI_INTERNAL_EV_HANDLER_DECL(key_press) {
      propagate = true;
    }
    // * key release
    CYDUI_INTERNAL_EV_HANDLER_DECL(key_release) {
      propagate = true;
    }

    // * text input
    CYDUI_INTERNAL_EV_HANDLER_DECL(text_input) {}

    // * focus changed input
    CYDUI_INTERNAL_EV_HANDLER_DECL(focus_changed) {}

    ElementVector draw_fragment CYDUI_INTERNAL_EV_fragment_ARGS {
      return {};
    }
  };

#pragma clang diagnostic pop

  template <typename ComponentBlueprint>
  struct event_handler_data_t: public event_handler_t {
    event_handler_data_t(
      TypedComponent<ComponentBlueprint>    component_,
      ComponentState&                       state_,
      fabric::async::async_bus_t&           bus_,
      typename ComponentBlueprint::props_t& props_,
      const style::style_base_t&            style_)
        : event_handler_t(*component_.get()),
          component(component_),
          state(state_),
          bus(bus_),
          props(props_),
          style(style_) {}

    static constexpr bool has_custom_state_type = false;
    static constexpr bool has_custom_style_type = false;

    TypedComponent<ComponentBlueprint>    component;
    ComponentState&                       state;
    fabric::async::async_bus_t&           bus;
    typename ComponentBlueprint::props_t& props;
    const style::style_base_t&            style;
  };
} // namespace cydui::detail

export namespace $self     = cydui::layout::anchors::self_component;
export namespace $parent   = cydui::layout::anchors::parent_component;
export namespace $previous = cydui::layout::anchors::previous_component;
