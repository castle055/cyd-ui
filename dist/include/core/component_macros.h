//
// Created by castle on 10/26/23.
//

#ifndef CYD_UI_COMPONENT_MACROS_H
#define CYD_UI_COMPONENT_MACROS_H

// ? Overridable macros in case of name collision
#define CYDUI_STATE_NAME(NAME) NAME##State
#define CYDUI_EV_HANDLER_NAME(NAME) NAME##EventHandler
#define CYDUI_EV_HANDLER_DATA_NAME(NAME) NAME##EventHandlerData



// ? Every component class must have this data
#define CYDUI_COMPONENT_METADATA(NAME_) \
    static constexpr std::string NAME = #NAME_ ; \



// ? Macros for declaring component classes
// ?>
#define STATE(NAME) \
struct CYDUI_STATE_NAME(NAME): public component_state_t


// ?>
#define COMPONENT(NAME, ...) \
struct CYDUI_EV_HANDLER_NAME(NAME); \
struct NAME:                 \
  public component_t<CYDUI_EV_HANDLER_NAME(NAME),NAME> \
  {                          \
    CYDUI_COMPONENT_METADATA(NAME) \
    using state_t = CYDUI_STATE_NAME(NAME);            \
    using event_handler_t = CYDUI_EV_HANDLER_NAME(NAME); \
    struct props_t           \
      __VA_ARGS__            \
    props{};                 \
    NAME() = default;        \
    explicit NAME(props_t props)    \
      : component_t<CYDUI_EV_HANDLER_NAME(NAME),NAME>()\
      , props(std::move(props)) { }     \
  };                         \
struct CYDUI_EV_HANDLER_DATA_NAME(NAME) {              \
  CYDUI_STATE_NAME(NAME)* state;                       \
  NAME::props_t* props;      \
};                           \
struct CYDUI_EV_HANDLER_NAME(NAME)                     \
  : public event_handler_t,  \
    public CYDUI_EV_HANDLER_DATA_NAME(NAME)


// ?>
#define STATE_TEMPLATE(NAME) \
template SET_COMPONENT_TEMPLATE   \
struct CYDUI_STATE_NAME(NAME): public component_state_t


// ?>
#define COMPONENT_TEMPLATE(NAME, ...) \
template SET_COMPONENT_TEMPLATE       \
struct CYDUI_EV_HANDLER_NAME(NAME);   \
template SET_COMPONENT_TEMPLATE       \
struct NAME:                          \
  public component_t<                 \
    CYDUI_EV_HANDLER_NAME(NAME) SET_COMPONENT_TEMPLATE_SHORT, \
    NAME SET_COMPONENT_TEMPLATE_SHORT \
  > {                                 \
    CYDUI_COMPONENT_METADATA(NAME)    \
    using state_t = CYDUI_STATE_NAME(NAME) SET_COMPONENT_TEMPLATE_SHORT; \
    using event_handler_t = CYDUI_EV_HANDLER_NAME(NAME) SET_COMPONENT_TEMPLATE_SHORT; \
    struct props_t                    \
      __VA_ARGS__                     \
    props{};                          \
    NAME() = default;                 \
    explicit NAME(props_t props)      \
      : component_t<                  \
        CYDUI_EV_HANDLER_NAME(NAME) SET_COMPONENT_TEMPLATE_SHORT,        \
        NAME SET_COMPONENT_TEMPLATE_SHORT                     \
      >()                             \
      , props(std::move(props)) { }              \
};                                    \
template SET_COMPONENT_TEMPLATE       \
struct CYDUI_EV_HANDLER_DATA_NAME(NAME) {          \
  CYDUI_STATE_NAME(NAME) SET_COMPONENT_TEMPLATE_SHORT* state; \
  NAME SET_COMPONENT_TEMPLATE_SHORT::props_t* props;          \
};                                    \
template SET_COMPONENT_TEMPLATE       \
struct CYDUI_EV_HANDLER_NAME(NAME)    \
  : public event_handler_t,           \
    public CYDUI_EV_HANDLER_DATA_NAME(NAME) SET_COMPONENT_TEMPLATE_SHORT

#endif //CYD_UI_COMPONENT_MACROS_H
