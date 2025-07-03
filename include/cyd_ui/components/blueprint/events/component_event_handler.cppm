// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include "./component_event_macros.h"

export module cydui.components.blueprint.event_handler;

export import std;

export import cydui.components.mounted;
export import cydui.components.handle;
export import cydui.geometry.anchors;
export import cydui.backends.frame_base;

export import cydui.events;


#define TO_STRING(...) #__VA_ARGS__
#define ANCHOR(PREFIX, NAME)                                                                       \
  struct NAME {                                                                                    \
    static constexpr dimension_parameter_t x{TO_STRING(PREFIX##_##NAME##_x)};                      \
    static constexpr dimension_parameter_t y{TO_STRING(PREFIX##_##NAME##_y)};                      \
  }

#define CYDUI_INTERNAL_EV_HANDLER_DECL(NAME) void on_##NAME CYDUI_INTERNAL_EV_##NAME##_ARGS

#define CYDUI_INTERNAL_EV_HANDLER_DECL_W_RET(NAME)                                                 \
  CYDUI_INTERNAL_EV_##NAME##_RETURN on_##NAME CYDUI_INTERNAL_EV_##NAME##_ARGS


export namespace cydui::components {


#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedMacroInspection"

  //! EVENT HANDLER STRUCT
  struct event_handler_t {
  private:
    mounted_component_t& component;

  public:
    const std::list<mounted_component_t::uptr>& $children;
    static constexpr bool                       handles_text_input = false;

    explicit event_handler_t(mounted_component_t& comp)
        : component(comp),
          $children(component.get_children()) {}

    // virtual ~event_handler_t() {}

    // * mount/dismount
    CYDUI_INTERNAL_EV_HANDLER_DECL(mount) {}
    CYDUI_INTERNAL_EV_HANDLER_DECL(dismount) {}

    CYDUI_INTERNAL_EV_HANDLER_DECL_W_RET(redraw) {
      return {};
    }

#define DIMENSIONAL_ARGS                                                                           \
  $x, $y, $width, $height, $padding_top, $padding_bottom, $padding_left, $padding_right

    // ? MOUSE EVENTS
#define CYDUI_INTERNAL_EV_button_PROPAGATE(NAME)                                                   \
  if (not component.is_root())                                                                     \
    component.get_parent()->get_event_dispatcher().dispatch_button_##NAME(button, x + $x, y + $y);

#define CYDUI_INTERNAL_EV_mouse_PROPAGATE(NAME)                                                    \
  if (not component.is_root())                                                                     \
    component.get_parent()->get_event_dispatcher().dispatch_mouse_##NAME(x + $x, y + $y);

    // * button press
    CYDUI_INTERNAL_EV_HANDLER_DECL(button_press) {
      CYDUI_INTERNAL_EV_button_PROPAGATE(press);
    }
    // * button release
    CYDUI_INTERNAL_EV_HANDLER_DECL(button_release) {
      CYDUI_INTERNAL_EV_button_PROPAGATE(release);
    }
    // * mouse enter
    CYDUI_INTERNAL_EV_HANDLER_DECL(mouse_enter) {}
    // * mouse exit
    CYDUI_INTERNAL_EV_HANDLER_DECL(mouse_exit) {}
    // * mouse motion
    CYDUI_INTERNAL_EV_HANDLER_DECL(mouse_motion) {
      CYDUI_INTERNAL_EV_mouse_PROPAGATE(motion);
    }
    // * mouse scroll
    CYDUI_INTERNAL_EV_HANDLER_DECL(scroll) {
      auto& style     = component.get_style();
      auto& geom      = component.get_geometry();
      bool  processed = false;
      if (style.overflow_x == overflow_e::SCROLL) {
        auto c_width = dimensions::get_value(geom.content_size[geometry::X_AXIS]);
        if ($width < c_width) {
          auto new_scroll = dimensions::get_value(geom.scroll[geometry::X_AXIS]) + dx;
          if (new_scroll < 0_px)
            new_scroll = 0_px;
          if (new_scroll > (c_width - $width))
            new_scroll = c_width - $width;
          component.scroll_x(new_scroll);
          processed = true;
        }
      }
      if (style.overflow_y == overflow_e::SCROLL) {
        auto c_height = dimensions::get_value(geom.content_size[geometry::Y_AXIS]);
        if ($height < c_height) {
          auto new_scroll = dimensions::get_value(geom.scroll[geometry::Y_AXIS])
                            - dy; // vertical scroll must be flipped
          if (new_scroll < 0_px)
            new_scroll = 0_px;
          if (new_scroll > (c_height - $height))
            new_scroll = c_height - $height;
          component.scroll_y(new_scroll);
          processed = true;
        }
      }
      if (processed) {
        component.mark_dirty();
      } else {
        // Propagate event to parent
        if (not component.is_root())
          component.get_parent()->get_event_dispatcher().dispatch_scroll(dx, dy);
      }
    }

    // ? KEYBOARD EVENTS
    // * key press
    CYDUI_INTERNAL_EV_HANDLER_DECL(key_press) {
      if (not component.is_root())
        component.get_parent()->get_event_dispatcher().dispatch_key_press(ev);
    }
    // * key release
    CYDUI_INTERNAL_EV_HANDLER_DECL(key_release) {
      if (not component.is_root())
        component.get_parent()->get_event_dispatcher().dispatch_key_release(ev);
    }

    // * text input
    CYDUI_INTERNAL_EV_HANDLER_DECL(text_input) {}

    // * focus changed input
    CYDUI_INTERNAL_EV_HANDLER_DECL(focus_changed) {}

    void draw_fragment CYDUI_INTERNAL_EV_fragment_ARGS {}
  };

#pragma clang diagnostic pop

  template <typename Component>
  struct event_handler_data_t: public event_handler_t {
    event_handler_data_t(
      component_handle_t<Component>             component_,
      const std::shared_ptr<component_state_t>& state_,
      const backends::frame_base::sptr&         window_,
      typename Component::props_t&              props_,
      const style::style_base_t&                style_
    )
        : event_handler_t(component_handle_delegate::get_ref(component_)),
          component(component_),
          state(*state_),
          window(*window_),
          props(props_),
          style(style_) {}

    static constexpr bool has_custom_state_type = false;
    static constexpr bool has_custom_style_type = false;

    component_handle_t<Component> component;
    component_state_t&            state;
    backends::frame_base&         window;
    typename Component::props_t&  props;
    const style::style_base_t&    style;

    using $self     = geometry::anchors::self_component;
    using $parent   = geometry::anchors::parent_component;
    using $previous = geometry::anchors::previous_component;
  };
} // namespace cydui::components
