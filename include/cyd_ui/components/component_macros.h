// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CYD_UI_COMPONENT_MACROS_H
#define CYD_UI_COMPONENT_MACROS_H

#include "./component_event_macros.h"


// ? Overridable macros in case of name collision
#define CYDUI_STATE_NAME(NAME) State##NAME
#define CYDUI_EV_HANDLER_NAME(NAME) EventHandler##NAME
#define CYDUI_EV_HANDLER_DATA_NAME(NAME) EventHandlerData##NAME



// ? Every component class must have this data
#define CYDUI_COMPONENT_METADATA(NAME_) \
    static constexpr const char* NAME = #NAME_ ; \
    std::string name() override { return std::string {NAME}; }

// ? Macros for declaring component classes
// ?>

//#define STATE(NAME) \
//struct CYDUI_STATE_NAME(NAME): public cyd::ui::components::component_state_t

#define STATE ; public: struct init: public cyd::ui::components::component_state_t

#define ATTRIBUTE(NAME, ...) \
  public: \
  auto& NAME (const __VA_ARGS__& value) { \
    NAME##_ = value; \
    return *this; \
  } \
private: \
  __VA_ARGS__ NAME##_

// ?>
#define COMPONENT_EXTRAS(...)                                                                      \
  __VA_ARGS__                                                                                      \
  }                                                                                                \
  ;

#define COMPONENT(NAME, ...)                                                                       \
  NAME;                                                                                            \
  struct CYDUI_EV_HANDLER_NAME(NAME);                                                              \
  struct NAME: public cyd::ui::components::component_t<CYDUI_EV_HANDLER_NAME(NAME), NAME> {        \
    CYDUI_COMPONENT_METADATA(NAME)                                                                 \
    using event_handler_t = CYDUI_EV_HANDLER_NAME(NAME);                                           \
    struct init;                                                                                   \
    struct props_t __VA_ARGS__;                                                                    \
                                                                                                   \
  public:                                                                                          \
    props_t props;                                                                                 \
    using state_t = typename std::conditional<                                                     \
      is_type_complete_v<struct init>,                                                             \
      init,                                                                                        \
      cyd::ui::components::component_state_t>::type;                                               \
    template <typename P = props_t>                                                                \
    explicit NAME(                                                                                 \
      typename std::enable_if<std::is_default_constructible_v<P>, props_t>::type props = {}        \
    )                                                                                              \
        : cyd::ui::components::component_t<CYDUI_EV_HANDLER_NAME(NAME), NAME>(),                   \
          props(std::move(props)) {}                                                               \
    explicit NAME(props_t props)                                                                   \
        : cyd::ui::components::component_t<CYDUI_EV_HANDLER_NAME(NAME), NAME>(),                   \
          props(std::move(props)) {}                                                               \
    ~NAME() override = default;                                                                    \
    void* get_props() override {                                                                   \
      return (void*)&(this->props);                                                                \
    }                                                                                              \
    friend struct CYDUI_EV_HANDLER_NAME(NAME);                                                     \
    friend struct cyd::ui::components::event_handler_data_t<NAME>;                                 \
  };                                                                                               \
  struct CYDUI_EV_HANDLER_NAME(NAME)                                                               \
      : public cyd::ui::components::event_handler_data_t<NAME>


// ?>
#define STATE_TEMPLATE(NAME) \
template SET_COMPONENT_TEMPLATE   \
struct CYDUI_STATE_NAME(NAME): public cyd::ui::components::component_state_t


// ?>
#define SET_COMPONENT_TEMPLATE_DEFAULT SET_COMPONENT_TEMPLATE

#define COMPONENT_TEMPLATE(NAME, ...) \
template SET_COMPONENT_TEMPLATE_DEFAULT \
struct CYDUI_EV_HANDLER_NAME(NAME);   \
template SET_COMPONENT_TEMPLATE_DEFAULT \
struct NAME:                          \
  public cyd::ui::components::component_t<          \
    CYDUI_EV_HANDLER_NAME(NAME) SET_COMPONENT_TEMPLATE_SHORT, \
    NAME SET_COMPONENT_TEMPLATE_SHORT \
  > {                                 \
    CYDUI_COMPONENT_METADATA(NAME)    \
    using state_t = CYDUI_STATE_NAME(NAME) SET_COMPONENT_TEMPLATE_SHORT; \
    using event_handler_t = CYDUI_EV_HANDLER_NAME(NAME) SET_COMPONENT_TEMPLATE_SHORT; \
    struct props_t                    \
      __VA_ARGS__;                    \
    props_t props;                    \
    NAME() = default;                 \
    explicit NAME(props_t props)      \
      : cyd::ui::components::component_t<           \
        CYDUI_EV_HANDLER_NAME(NAME) SET_COMPONENT_TEMPLATE_SHORT,        \
        NAME SET_COMPONENT_TEMPLATE_SHORT                     \
      >()                             \
      , props(std::move(props)) { }   \
    ~NAME() override = default;       \
    void* get_props() override {      \
      return (void*)&(this->props);   \
    }                                 \
};                                    \
template SET_COMPONENT_TEMPLATE_DEFAULT \
struct CYDUI_EV_HANDLER_DATA_NAME(NAME) {                     \
  std::shared_ptr<CYDUI_STATE_NAME(NAME) SET_COMPONENT_TEMPLATE_SHORT> state = nullptr;  \
  cyd::fabric::async::async_bus_t* window = nullptr;       \
  NAME SET_COMPONENT_TEMPLATE_SHORT::props_t* props = nullptr;\
  attrs_component<NAME SET_COMPONENT_TEMPLATE_SHORT>* attrs = nullptr;   \
  logging::logger log{.name = #NAME}; \
};                                    \
template SET_COMPONENT_TEMPLATE       \
struct CYDUI_EV_HANDLER_NAME(NAME)    \
  : public cyd::ui::components::event_handler_t,    \
    public CYDUI_EV_HANDLER_DATA_NAME(NAME) SET_COMPONENT_TEMPLATE_SHORT



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

#define ON_CUSTOM_EVENTS(...) \
std::unordered_map<std::string, listener_data_t> get_event_listeners() { \
  _Pragma("clang diagnostic push") \
  _Pragma("clang diagnostic ignored \"-Wunused-lambda-capture\"") \
  return { __VA_ARGS__ };     \
  _Pragma("clang diagnostic pop") \
}

#define ON_EVENT(EVENT, ...) \
{ EVENT ::type, {[&](cyd::fabric::async::event_t ev) { \
  if (nullptr == state) return;                     \
  auto parsed_event = ev->parse<EVENT>(); \
  [&](const cyd::fabric::async::ParsedEvent<EVENT>::DataType* ev) \
    __VA_ARGS__              \
  (parsed_event.data);       \
}}}

#define FRAGMENT void draw_fragment CYDUI_INTERNAL_EV_fragment_ARGS


#endif //CYD_UI_COMPONENT_MACROS_H
