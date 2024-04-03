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
    logging::logger log{.name = #NAME_};\
    static constexpr const char* NAME = #NAME_ ; \
    std::string name() override { return std::string {NAME}; }

// ? Macros for declaring component classes
// ?>

//#define STATE(NAME) \
//struct CYDUI_STATE_NAME(NAME): public cydui::components::component_state_t

#define STATE ; struct init: public cydui::components::component_state_t

// ?>
#define COMPONENT(NAME, ...) \
struct NAME;                 \
struct CYDUI_EV_HANDLER_NAME(NAME); \
struct NAME:                 \
  public cydui::components::component_t<CYDUI_EV_HANDLER_NAME(NAME),NAME> \
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
        , cydui::components::component_state_t>::type;                    \
    NAME() = default;        \
    explicit NAME(props_t props)    \
      : cydui::components::component_t<CYDUI_EV_HANDLER_NAME(NAME),NAME>()\
      , props(std::move(props)) { } \
    ~NAME() override = default;     \
    void* get_props() override {    \
      return (void*)&(this->props); \
    }                        \
  };                         \
struct CYDUI_EV_HANDLER_DATA_NAME(NAME) {                                 \
  NAME::state_t* state = nullptr;   \
  cydui::window::CWindow* window = nullptr;                               \
  NAME::props_t* props = nullptr;   \
  attrs_component<NAME>* attrs = nullptr;                                 \
  logging::logger log{.name = #NAME};                                     \
};                           \
struct CYDUI_EV_HANDLER_NAME(NAME)  \
  : public cydui::components::event_handler_t,                            \
    public CYDUI_EV_HANDLER_DATA_NAME(NAME)




// ?>
#define STATE_TEMPLATE(NAME) \
template SET_COMPONENT_TEMPLATE   \
struct CYDUI_STATE_NAME(NAME): public cydui::components::component_state_t


// ?>
#define SET_COMPONENT_TEMPLATE_DEFAULT SET_COMPONENT_TEMPLATE

#define COMPONENT_TEMPLATE(NAME, ...) \
template SET_COMPONENT_TEMPLATE_DEFAULT \
struct CYDUI_EV_HANDLER_NAME(NAME);   \
template SET_COMPONENT_TEMPLATE_DEFAULT \
struct NAME:                          \
  public cydui::components::component_t<          \
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
      : cydui::components::component_t<           \
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
  CYDUI_STATE_NAME(NAME) SET_COMPONENT_TEMPLATE_SHORT* state = nullptr;  \
  NAME SET_COMPONENT_TEMPLATE_SHORT::props_t* props = nullptr;\
  attrs_component<NAME SET_COMPONENT_TEMPLATE_SHORT>* attrs = nullptr;   \
  logging::logger log{.name = #NAME}; \
};                                    \
template SET_COMPONENT_TEMPLATE       \
struct CYDUI_EV_HANDLER_NAME(NAME)    \
  : public cydui::components::event_handler_t,    \
    public CYDUI_EV_HANDLER_DATA_NAME(NAME) SET_COMPONENT_TEMPLATE_SHORT

#endif //CYD_UI_COMPONENT_MACROS_H
