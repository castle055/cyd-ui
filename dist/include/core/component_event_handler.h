//
// Created by castle on 10/26/23.
//

#ifndef CYD_UI_COMPONENT_EVENT_HANDLER_H
#define CYD_UI_COMPONENT_EVENT_HANDLER_H

#include "component_holder.h"

#include "../events/cydui_events.h"
#include "../cydstd/pragma.h"

#include <vector>

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
// ! }

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedMacroInspection"

// ? EVENT HANDLER STRUCT
struct event_handler_t {
  event_handler_t() = default;
  virtual ~event_handler_t() = default;


#define CYDUI_INTERNAL_EV_redraw_RETURN std::vector<component_holder_t>
#define CYDUI_INTERNAL_EV_redraw_ARGS   ()
  CYDUI_INTERNAL_EV_HANDLER_DECL_W_RET(redraw) {return {};}
  
  // ? MOUSE EVENTS
#define CYDUI_INTERNAL_EV_button_ARGS (Button button, int x, int y)
#define CYDUI_INTERNAL_EV_mouse_ARGS  (int x, int y)
  // * button press
#define CYDUI_INTERNAL_EV_button_press_ARGS       CYDUI_INTERNAL_EV_button_ARGS
  CYDUI_INTERNAL_EV_HANDLER_DECL(button_press) { }
  // * button release
#define CYDUI_INTERNAL_EV_button_release_ARGS     CYDUI_INTERNAL_EV_button_ARGS
  CYDUI_INTERNAL_EV_HANDLER_DECL(button_release) { }
  // * mouse enter
#define CYDUI_INTERNAL_EV_mouse_enter_ARGS        CYDUI_INTERNAL_EV_mouse_ARGS
  CYDUI_INTERNAL_EV_HANDLER_DECL(mouse_enter) { }
  // * mouse exit
#define CYDUI_INTERNAL_EV_mouse_exit_ARGS         CYDUI_INTERNAL_EV_mouse_ARGS
  CYDUI_INTERNAL_EV_HANDLER_DECL(mouse_exit) { }
  // * mouse motion
#define CYDUI_INTERNAL_EV_mouse_motion_ARGS       CYDUI_INTERNAL_EV_mouse_ARGS
  CYDUI_INTERNAL_EV_HANDLER_DECL(mouse_motion) { }
  
  // ? KEYBOARD EVENTS
  // * key press
#define CYDUI_INTERNAL_EV_key_press_ARGS          (Key key)
  CYDUI_INTERNAL_EV_HANDLER_DECL(key_press) { }
  // * key release
#define CYDUI_INTERNAL_EV_key_release_ARGS        (Key key)
  CYDUI_INTERNAL_EV_HANDLER_DECL(key_release) { }
  // * set text input context (a type that accepts text events like insert, backspace and such)
  TODO("Implement a method to set an input context for text input")
  
  struct listener_data_t {
    std::function<void(cydui::events::Event*)> handler {};
  };
  virtual std::unordered_map<std::string, listener_data_t> get_event_listeners() {
    return {};
  }
};

#define ON_REDRAW           CYDUI_INTERNAL_EV_HANDLER_IMPL_W_RET(redraw)
#define ON_BUTTON_PRESS     CYDUI_INTERNAL_EV_HANDLER_IMPL_W_RET(button_press)
#define ON_BUTTON_RELEASE   CYDUI_INTERNAL_EV_HANDLER_IMPL_W_RET(button_release)
#define ON_MOUSE_ENTER      CYDUI_INTERNAL_EV_HANDLER_IMPL_W_RET(mouse_enter)
#define ON_MOUSE_EXIT       CYDUI_INTERNAL_EV_HANDLER_IMPL_W_RET(mouse_exit)
#define ON_MOUSE_MOTION     CYDUI_INTERNAL_EV_HANDLER_IMPL_W_RET(mouse_motion)
#define ON_KEY_PRESS        CYDUI_INTERNAL_EV_HANDLER_IMPL_W_RET(key_press)
#define ON_KEY_RELEASE      CYDUI_INTERNAL_EV_HANDLER_IMPL_W_RET(key_release)
TODO("Implement a method to set an input context for text input")

#pragma clang diagnostic pop

#define ON_CUSTOM_EVENTS(...) \
std::unordered_map<std::string, listener_data_t> get_event_listeners() override { \
  _Pragma("clang diagnostic push") \
  _Pragma("clang diagnostic ignored \"-Wunused-lambda-capture\"") \
  return __VA_ARGS__ ;                                                            \
  _Pragma("clang diagnostic pop") \
}

#define ON_EVENT(EVENT, ...) \
{ EVENT ::type, {[this](cydui::events::Event* ev) { \
  auto parsed_event = ev->parse<EVENT>(); \
  [this](const cydui::events::ParsedEvent<EVENT>::DataType* ev) \
    __VA_ARGS__              \
  (parsed_event.data);       \
}}}

#endif //CYD_UI_COMPONENT_EVENT_HANDLER_H
