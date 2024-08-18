// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include "./component_event_macros.h"
#include <cyd_fabric/async/async_bus.h>

export module cydui.components:event_handler;

export import :holder;

export import cydui.events;
export import cydui.graphics;

#include <vector>

export {
namespace cyd::ui::components {
// ? INTERNAL MACROS
// ! DO NOT USE {
#define CYDUI_INTERNAL_EV_HANDLER_DECL(NAME) \
  virtual                                    \
  void                                       \
  on_##NAME                                  \
  CYDUI_INTERNAL_EV_##NAME##_ARGS            \

#define CYDUI_INTERNAL_EV_HANDLER_DECL_W_RET(NAME) \
  virtual                                          \
  CYDUI_INTERNAL_EV_##NAME##_RETURN                \
  on_##NAME                                        \
  CYDUI_INTERNAL_EV_##NAME##_ARGS                  \

#define CYDUI_INTERNAL_EV_HANDLER_DIMENSION_ACCESSOR(NAME) \
  inline auto& $##NAME() const                             \
    { return get_dim().NAME; }

#define CYDUI_INTERNAL_EV_HANDLER_DIMENSION_MUTATOR(NAME) \
  inline auto& $##NAME(auto value) const {                \
    auto d = get_dim();                                  \
    d.NAME = value;                                       \
    d.fixed_##NAME = true;                                  \
    return get_dim().NAME;                                \
  }
// ! }

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedMacroInspection"

// ? EVENT HANDLER STRUCT
    struct event_handler_t {
      event_handler_t* parent;

      std::function<component_dimensional_relations_t()> get_dim;
      std::function<std::vector<component_base_t*>()> $children;
      CYDUI_INTERNAL_EV_HANDLER_DIMENSION_ACCESSOR(x)
      CYDUI_INTERNAL_EV_HANDLER_DIMENSION_ACCESSOR(y)
      CYDUI_INTERNAL_EV_HANDLER_DIMENSION_ACCESSOR(w)
      CYDUI_INTERNAL_EV_HANDLER_DIMENSION_ACCESSOR(h)
      CYDUI_INTERNAL_EV_HANDLER_DIMENSION_MUTATOR(w)
      CYDUI_INTERNAL_EV_HANDLER_DIMENSION_MUTATOR(h)
      CYDUI_INTERNAL_EV_HANDLER_DIMENSION_ACCESSOR(cx)
      CYDUI_INTERNAL_EV_HANDLER_DIMENSION_ACCESSOR(cy)
      CYDUI_INTERNAL_EV_HANDLER_DIMENSION_ACCESSOR(cw)
      CYDUI_INTERNAL_EV_HANDLER_DIMENSION_ACCESSOR(ch)
      CYDUI_INTERNAL_EV_HANDLER_DIMENSION_ACCESSOR(margin_top)
      CYDUI_INTERNAL_EV_HANDLER_DIMENSION_ACCESSOR(margin_bottom)
      CYDUI_INTERNAL_EV_HANDLER_DIMENSION_ACCESSOR(margin_left)
      CYDUI_INTERNAL_EV_HANDLER_DIMENSION_ACCESSOR(margin_right)
      CYDUI_INTERNAL_EV_HANDLER_DIMENSION_ACCESSOR(padding_top)
      CYDUI_INTERNAL_EV_HANDLER_DIMENSION_ACCESSOR(padding_bottom)
      CYDUI_INTERNAL_EV_HANDLER_DIMENSION_ACCESSOR(padding_left)
      CYDUI_INTERNAL_EV_HANDLER_DIMENSION_ACCESSOR(padding_right)

#undef CYDUI_INTERNAL_EV_HANDLER_DIMENSION_ACCESSOR
#undef CYDUI_INTERNAL_EV_HANDLER_DIMENSION_MUTATOR
      
      event_handler_t() = default;
      virtual ~event_handler_t() = default;


      CYDUI_INTERNAL_EV_HANDLER_DECL_W_RET(redraw) {return {};}
      
      // ? MOUSE EVENTS
#define CYDUI_INTERNAL_EV_button_PROPAGATE(NAME) \
  if (parent)                                   \
    parent->on_button_##NAME(button, x + $x().val(), y + $y().val())
#define CYDUI_INTERNAL_EV_mouse_PROPAGATE(NAME) \
  if (parent)                                   \
    parent->on_mouse_##NAME(x + $x().val(), y + $y().val())
      
      // * button press
      CYDUI_INTERNAL_EV_HANDLER_DECL(button_press) {
        CYDUI_INTERNAL_EV_button_PROPAGATE(press);
      }
      // * button release
      CYDUI_INTERNAL_EV_HANDLER_DECL(button_release) {
        CYDUI_INTERNAL_EV_button_PROPAGATE(release);
      }
      // * mouse enter
      CYDUI_INTERNAL_EV_HANDLER_DECL(mouse_enter) {
        CYDUI_INTERNAL_EV_mouse_PROPAGATE(enter);
      }
      // * mouse exit
      CYDUI_INTERNAL_EV_HANDLER_DECL(mouse_exit) {
        CYDUI_INTERNAL_EV_mouse_PROPAGATE(exit);
      }
      // * mouse motion
      CYDUI_INTERNAL_EV_HANDLER_DECL(mouse_motion) {
        CYDUI_INTERNAL_EV_mouse_PROPAGATE(motion);
      }
      // * mouse scroll
      CYDUI_INTERNAL_EV_HANDLER_DECL(scroll) {
        if (parent)
          parent->on_scroll(dx, dy);
      }
      
      // ? KEYBOARD EVENTS
      // * key press
      CYDUI_INTERNAL_EV_HANDLER_DECL(key_press) {
        if (parent)
          parent->on_key_press(ev);
      }
      // * key release
      CYDUI_INTERNAL_EV_HANDLER_DECL(key_release) {
        if (parent)
          parent->on_key_release(ev);
      }
      // * set text input context (a type that accepts text events like insert, backspace and such)
      //TODO - Implement a method to set an input context for text input
      
      struct listener_data_t {
        std::function<void(cyd::fabric::async::event_t)> handler {};
      };
      virtual std::unordered_map<std::string, listener_data_t> get_event_listeners() {
        return {};
      }
      
      virtual void draw_fragment(vg::vg_fragment_t &fragment) {
      
      }
    };

#pragma clang diagnostic pop

}

}
