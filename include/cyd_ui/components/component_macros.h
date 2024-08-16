// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CYD_UI_COMPONENT_MACROS_H
#define CYD_UI_COMPONENT_MACROS_H

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
; public: \
  auto& NAME (const __VA_ARGS__& value) { \
    NAME##_ = value; \
    return *this; \
  } \
private: \
  __VA_ARGS__ NAME##_

// ?>
#define COMPONENT(NAME, ...) \
struct NAME;                 \
struct CYDUI_EV_HANDLER_NAME(NAME); \
struct NAME:                 \
  public cyd::ui::components::component_t<CYDUI_EV_HANDLER_NAME(NAME),NAME> \
  {                          \
    CYDUI_COMPONENT_METADATA(NAME)  \
    using event_handler_t = CYDUI_EV_HANDLER_NAME(NAME);                  \
    struct init;             \
    struct props_t           \
      __VA_ARGS__;           \
    props_t props;           \
    using state_t =          \
      std::conditional<is_type_complete_v<struct init>                    \
        , init               \
        , cyd::ui::components::component_state_t>::type;                    \
    NAME() = default;        \
    explicit NAME(props_t props)    \
      : cyd::ui::components::component_t<CYDUI_EV_HANDLER_NAME(NAME),NAME>()\
      , props(std::move(props)) { } \
    ~NAME() override = default;     \
    void* get_props() override {    \
      return (void*)&(this->props); \
    }                        \
  };                         \
struct CYDUI_EV_HANDLER_DATA_NAME(NAME) {                                 \
  std::shared_ptr<NAME::state_t> state = nullptr;   \
  cyd::ui::window::CWindow* window = nullptr;                               \
  NAME::props_t* props = nullptr;   \
  attrs_component<NAME>* attrs = nullptr;                                 \
  NAME* component_instance() const { return static_cast<NAME*>(this->state->component_instance.value()); } \
};                           \
struct CYDUI_EV_HANDLER_NAME(NAME)  \
  : public cyd::ui::components::event_handler_t,                            \
    public CYDUI_EV_HANDLER_DATA_NAME(NAME)




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
  cyd::ui::window::CWindow* window = nullptr;       \
  NAME SET_COMPONENT_TEMPLATE_SHORT::props_t* props = nullptr;\
  attrs_component<NAME SET_COMPONENT_TEMPLATE_SHORT>* attrs = nullptr;   \
  logging::logger log{.name = #NAME}; \
};                                    \
template SET_COMPONENT_TEMPLATE       \
struct CYDUI_EV_HANDLER_NAME(NAME)    \
  : public cyd::ui::components::event_handler_t,    \
    public CYDUI_EV_HANDLER_DATA_NAME(NAME) SET_COMPONENT_TEMPLATE_SHORT

#endif //CYD_UI_COMPONENT_MACROS_H
