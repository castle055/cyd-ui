// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CYD_UI_COMPONENT_MACROS_H
#define CYD_UI_COMPONENT_MACROS_H

#include "./component_event_macros.h"
#include <SDL3/SDL_keycode.h>

#define STATE ; public: struct state_type: public cydui::components::component_state_t

#define EXTENDS(...) , __VA_ARGS__

#define COMPONENT_DECL(NAME, ...)                                                                  \
  struct NAME: public cydui::components::component_t<NAME> {                                       \
    struct event_handler_t;                                                                        \
    struct state_type;                                                                             \
    struct style_type;                                                                             \
    struct props_t __VA_ARGS__;                                                                    \
                                                                                                   \
  public:                                                                                          \
    props_t props;                                                                                 \
    using window_type = cydui::CWindow;                                                            \
    using state_t     = std::conditional_t<                                                        \
          is_type_complete_v<struct state_type>,                                                   \
          state_type,                                                                              \
          cydui::components::component_state_t>;                                                   \
    using style_t = std::conditional_t<                                                            \
      is_type_complete_v<struct style_type>,                                                       \
      style_type,                                                                                  \
      cydui::components::simple_style_t>;                                                          \
    template <typename P = props_t>                                                                \
    explicit NAME(std::enable_if_t<std::is_default_constructible_v<P>, props_t> props = {})        \
        : cydui::components::component_t<NAME>(),                                                  \
          props(std::move(props)) {}                                                               \
    explicit NAME(props_t props)                                                                   \
        : cydui::components::component_t<NAME>(),                                                  \
          props(std::move(props)) {}                                                               \
    ~NAME() override = default;                                                                    \
    void* get_props() override {                                                                   \
      return (void*)&(this->props);                                                                \
    }                                                                                              \
    auto& style(const std::function<void(style_t&)>& style_transform) {                            \
      this->set_style_transform(style_transform);                                                  \
      return *this;                                                                                \
    }                                                                                              \
    friend struct event_handler_t;                                                                 \
    friend struct cydui::components::event_handler_data_t<NAME>;                                   \
  };

#define COMPONENT_IMPL(NAME)                                                                       \
  struct NAME::event_handler_t: public cydui::components::event_handler_data_t<NAME>

#define COMPONENT(NAME, ...)                                                                       \
  COMPONENT_DECL(NAME, __VA_ARGS__)                                                                \
  COMPONENT_IMPL(NAME)


#define TCOMPONENT(NAME, ...)                                                                      \
  template NAME##_TEMPLATE struct NAME                                                             \
      : public cydui::components::component_t<NAME NAME##_TEMPLATE_SHORT> {                        \
    struct event_handler_t;                                                                        \
    struct state_type;                                                                             \
    struct style_type;                                                                             \
    struct props_t __VA_ARGS__;                                                                    \
                                                                                                   \
  public:                                                                                          \
    props_t props;                                                                                 \
    using window_type = cydui::CWindow;                                                            \
    using state_t     = std::conditional_t<                                                        \
          is_type_complete_v<struct state_type>,                                                   \
          state_type,                                                                              \
          cydui::components::component_state_t>;                                                   \
    using style_t = std::conditional_t<                                                            \
      is_type_complete_v<struct style_type>,                                                       \
      style_type,                                                                                  \
      cydui::components::simple_style_t>;                                                          \
    template <typename P = props_t>                                                                \
    explicit NAME(std::enable_if_t<std::is_default_constructible_v<P>, props_t> props = {})        \
        : cydui::components::component_t<NAME NAME##_TEMPLATE_SHORT>(),                            \
          props(std::move(props)) {}                                                               \
    explicit NAME(props_t props)                                                                   \
        : cydui::components::component_t<NAME NAME##_TEMPLATE_SHORT>(),                            \
          props(std::move(props)) {}                                                               \
    ~NAME() override = default;                                                                    \
    void* get_props() override {                                                                   \
      return (void*)&(this->props);                                                                \
    }                                                                                              \
    auto& style(std::function<void(style_t&)> style_transform) {                                   \
      this->set_style_transform(style_transform);                                                  \
      return *this;                                                                                \
    }                                                                                              \
    friend struct event_handler_t;                                                                 \
    friend struct cydui::components::event_handler_data_t<NAME NAME##_TEMPLATE_SHORT>;             \
  };                                                                                               \
  template NAME##_TEMPLATE struct NAME NAME##_TEMPLATE_SHORT::event_handler_t                      \
      : public cydui::components::event_handler_data_t<NAME NAME##_TEMPLATE_SHORT>


#define CYDUI_INTERNAL_EV_HANDLER_IMPL(NAME) \
  void                                       \
  on_##NAME                                  \
  CYDUI_INTERNAL_EV_##NAME##_ARGS

#define CYDUI_INTERNAL_EV_HANDLER_IMPL_W_RET(NAME) \
  CYDUI_INTERNAL_EV_##NAME##_RETURN                \
  on_##NAME                                        \
  CYDUI_INTERNAL_EV_##NAME##_ARGS

#define ON_REDRAW           CYDUI_INTERNAL_EV_HANDLER_IMPL_W_RET(redraw)
#define CHILDREN            CYDUI_INTERNAL_EV_HANDLER_IMPL_W_RET(redraw)
#define ON_BUTTON_PRESS     CYDUI_INTERNAL_EV_HANDLER_IMPL(button_press)
#define ON_BUTTON_RELEASE   CYDUI_INTERNAL_EV_HANDLER_IMPL(button_release)
#define ON_MOUSE_ENTER      CYDUI_INTERNAL_EV_HANDLER_IMPL(mouse_enter)
#define ON_MOUSE_EXIT       CYDUI_INTERNAL_EV_HANDLER_IMPL(mouse_exit)
#define ON_MOUSE_MOTION     CYDUI_INTERNAL_EV_HANDLER_IMPL(mouse_motion)
#define ON_SCROLL           CYDUI_INTERNAL_EV_HANDLER_IMPL(scroll)
#define ON_KEY_PRESS        CYDUI_INTERNAL_EV_HANDLER_IMPL(key_press)
#define ON_KEY_RELEASE      CYDUI_INTERNAL_EV_HANDLER_IMPL(key_release)
#define ON_FOCUS_CHANGED    CYDUI_INTERNAL_EV_HANDLER_IMPL(focus_changed)
#define ON_TEXT_INPUT                                                                              \
  static constexpr bool handles_text_input = true;                                                 \
  CYDUI_INTERNAL_EV_HANDLER_IMPL(text_input)

#define ON_EVENT(EVENT, ...) \
  custom_event_listener< EVENT > on_##EVENT{&window, [&](const EVENT& event) { \
    __VA_ARGS__ ;\
  }, [&](){state.force_redraw();}};

#define FRAGMENT void draw_fragment CYDUI_INTERNAL_EV_fragment_ARGS

#define SIGNAL(NAME, ...) \
private: \
  fabric::wiring::signal<__VA_ARGS__> NAME{}; \
public: \
  auto& on_##NAME(auto&& fun) { \
    NAME.connect(fun); \
    return *this; \
  }

#define ATTRIBUTE(NAME, ...) \
public: \
  auto& NAME (const __VA_ARGS__& value) { \
    NAME##_ = value; \
    return *this; \
  } \
private: \
__VA_ARGS__ NAME##_

#define STYLE struct style_type: cydui::components::style_base_t


//! ANIMATIONS

#define ANONYMOUS_STRUCT(...)   \
decltype([&] {                  \
    struct _anon_ __VA_ARGS__;  \
    return _anon_{};            \
}())

#define KEYFRAME(POS, ...)      cydui::keyframe::make(POS, __VA_ARGS__)
#define AUTO_KEYFRAME(POS, ...) cydui::keyframe::make(POS, ANONYMOUS_STRUCT(__VA_ARGS__){})

#endif //CYD_UI_COMPONENT_MACROS_H
