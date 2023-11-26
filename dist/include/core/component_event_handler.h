//
// Created by castle on 10/26/23.
//

#ifndef CYD_UI_COMPONENT_EVENT_HANDLER_H
#define CYD_UI_COMPONENT_EVENT_HANDLER_H

#include "../cydstd/pragma.h"
#include "component_holder.h"


#include "../events/cydui_events.h"
#include "../graphics/graphics.hpp"

#include <vector>

namespace cydui::components {
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

#define CYDUI_INTERNAL_EV_HANDLER_IMPL(NAME) \
  void                                       \
  on_##NAME                                  \
  CYDUI_INTERNAL_EV_##NAME##_ARGS            \
  override

#define CYDUI_INTERNAL_EV_HANDLER_IMPL_W_RET(NAME) \
  CYDUI_INTERNAL_EV_##NAME##_RETURN                \
  on_##NAME                                        \
  CYDUI_INTERNAL_EV_##NAME##_ARGS                  \
  override

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


#define CYDUI_INTERNAL_EV_redraw_RETURN std::vector<cydui::components::component_holder_t>
#define CYDUI_INTERNAL_EV_redraw_ARGS   ()
      CYDUI_INTERNAL_EV_HANDLER_DECL_W_RET(redraw) {return {};}
      
      // ? MOUSE EVENTS
#define CYDUI_INTERNAL_EV_button_ARGS (Button button, int x, int y)
#define CYDUI_INTERNAL_EV_mouse_ARGS  (int x, int y)
#define CYDUI_INTERNAL_EV_scroll_ARGS  (int dx, int dy)

#define CYDUI_INTERNAL_EV_button_PROPAGATE(NAME) \
  if (parent)                                   \
    parent->on_button_##NAME(button, x + $x().val(), y + $y().val())
#define CYDUI_INTERNAL_EV_mouse_PROPAGATE(NAME) \
  if (parent)                                   \
    parent->on_mouse_##NAME(x + $x().val(), y + $y().val())
      
      // * button press
#define CYDUI_INTERNAL_EV_button_press_ARGS       CYDUI_INTERNAL_EV_button_ARGS
      CYDUI_INTERNAL_EV_HANDLER_DECL(button_press) {
        CYDUI_INTERNAL_EV_button_PROPAGATE(press);
      }
      // * button release
#define CYDUI_INTERNAL_EV_button_release_ARGS     CYDUI_INTERNAL_EV_button_ARGS
      CYDUI_INTERNAL_EV_HANDLER_DECL(button_release) {
        CYDUI_INTERNAL_EV_button_PROPAGATE(release);
      }
      // * mouse enter
#define CYDUI_INTERNAL_EV_mouse_enter_ARGS        CYDUI_INTERNAL_EV_mouse_ARGS
      CYDUI_INTERNAL_EV_HANDLER_DECL(mouse_enter) {
        CYDUI_INTERNAL_EV_mouse_PROPAGATE(enter);
      }
      // * mouse exit
#define CYDUI_INTERNAL_EV_mouse_exit_ARGS         CYDUI_INTERNAL_EV_mouse_ARGS
      CYDUI_INTERNAL_EV_HANDLER_DECL(mouse_exit) {
        CYDUI_INTERNAL_EV_mouse_PROPAGATE(exit);
      }
      // * mouse motion
#define CYDUI_INTERNAL_EV_mouse_motion_ARGS       CYDUI_INTERNAL_EV_mouse_ARGS
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
#define CYDUI_INTERNAL_EV_key_press_ARGS          (const KeyEvent::DataType& ev)
      CYDUI_INTERNAL_EV_HANDLER_DECL(key_press) {
        if (parent)
          parent->on_key_press(ev);
      }
      // * key release
#define CYDUI_INTERNAL_EV_key_release_ARGS        (const KeyEvent::DataType& ev)
      CYDUI_INTERNAL_EV_HANDLER_DECL(key_release) {
        if (parent)
          parent->on_key_release(ev);
      }
      // * set text input context (a type that accepts text events like insert, backspace and such)
      TODO("Implement a method to set an input context for text input")
      
      struct listener_data_t {
        std::function<void(cydui::events::Event*)> handler {};
      };
      virtual std::unordered_map<std::string, listener_data_t> get_event_listeners() {
        return {};
      }
      
      virtual void draw_fragment(graphics::vg::vg_fragment_t &fragment) {
      
      }
    };

#define ON_REDRAW           CYDUI_INTERNAL_EV_HANDLER_IMPL_W_RET(redraw)
#define ON_BUTTON_PRESS     CYDUI_INTERNAL_EV_HANDLER_IMPL(button_press)
#define ON_BUTTON_RELEASE   CYDUI_INTERNAL_EV_HANDLER_IMPL(button_release)
#define ON_MOUSE_ENTER      CYDUI_INTERNAL_EV_HANDLER_IMPL(mouse_enter)
#define ON_MOUSE_EXIT       CYDUI_INTERNAL_EV_HANDLER_IMPL(mouse_exit)
#define ON_MOUSE_MOTION     CYDUI_INTERNAL_EV_HANDLER_IMPL(mouse_motion)
#define ON_SCROLL           CYDUI_INTERNAL_EV_HANDLER_IMPL(scroll)
#define ON_KEY_PRESS        CYDUI_INTERNAL_EV_HANDLER_IMPL(key_press)
#define ON_KEY_RELEASE      CYDUI_INTERNAL_EV_HANDLER_IMPL(key_release)
    TODO("Implement a method to set an input context for text input")

#pragma clang diagnostic pop

#define ON_CUSTOM_EVENTS(...) \
std::unordered_map<std::string, listener_data_t> get_event_listeners() override { \
  _Pragma("clang diagnostic push") \
  _Pragma("clang diagnostic ignored \"-Wunused-lambda-capture\"") \
  return { __VA_ARGS__ };     \
  _Pragma("clang diagnostic pop") \
}

#define ON_EVENT(EVENT, ...) \
{ EVENT ::type, {[&](cydui::events::Event* ev) { \
  if (nullptr == state) return;                     \
  auto parsed_event = ev->parse<EVENT>(); \
  [&](const cydui::events::ParsedEvent<EVENT>::DataType* ev) \
    __VA_ARGS__              \
  (parsed_event.data);       \
}}}

#define FRAGMENT \
  void           \
  draw_fragment  \
  (vg::vg_fragment_t& fragment) \
  override
  
}

#endif //CYD_UI_COMPONENT_EVENT_HANDLER_H
