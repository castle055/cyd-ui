//
// Created by castle on 8/21/22.
//

#ifndef CYD_UI_COMPONENTS_HPP
#define CYD_UI_COMPONENTS_HPP

#include "events.hpp"
#include "colors.hpp"
#include "properties.hpp"
#include "component_geometry.hpp"
#include "children_state_collection.hpp"
#include "window_types.hpp"
#include <vector>
#include <unordered_set>
#include <functional>

namespace cydui::components {
  class Component;
  
  class ComponentBorder {
  public:
    bool enabled = false;
    layout::color::Color* color = new layout::color::Color("#FCAE1E");
  };
  
  class ComponentState {
  public:
    ComponentState();
    
    Component* component_instance;
    bool stateless_comp = false;
    //int  x       = 0,
    //     y       = 0,
    //     w       = 0,
    //     h       = 0;
    bool focused        = false;
    
    // TODO - maybe init with 'true'? so it renders right away
    bool _dirty = false;
    
    void dirty();
    
    ComponentGeometry       geom;
    ComponentBorder         border;
    ChildrenStateCollection children;
  };
  
  class Component {
    
    std::function<void(Component*)> inner_redraw = nullptr;
    
    virtual void on_render(events::layout::CLayoutEvent* ev);
    
    virtual void on_redraw(events::layout::CLayoutEvent* ev);
    
    virtual void on_mouse_enter(events::layout::CLayoutEvent* ev);
    
    virtual void on_mouse_exit(events::layout::CLayoutEvent* ev);
    
    virtual void on_mouse_click(events::layout::CLayoutEvent* ev);
    
    virtual void on_scroll(events::layout::CLayoutEvent* ev);
    
    virtual void on_key_press(events::layout::CLayoutEvent* ev);
    
    virtual void on_key_release(events::layout::CLayoutEvent* ev);
  
  protected:
    // Private size operations
  public:
    //explicit Component(std::unordered_set<Component*> children);
    
    //explicit Component(ComponentState* state, std::unordered_set<Component*> children);
    
    Component();
    
    Component(std::function<void(Component*)> inner);
    
    Component(ComponentState* state);
    
    Component(ComponentState* state, std::function<void(Component*)> inner);
    
    virtual ~Component();
    
    Component     * parent;
    ComponentState* state;
    
    void add(std::vector<Component*> children);
    
    std::vector<Component*> children;
    
    void on_event(events::layout::CLayoutEvent* ev);

    void redraw();

    void render(const cydui::window::CWindow* win);

    Component* get_parent();
    
    Component* set_size(int w, int h);
    
    Component* set_size(IntProperty* w, IntProperty* h);
    
    Component* set_size(IntProperty::IntBinding w, IntProperty::IntBinding h);
    
    
    Component* set_width(int w);
    
    //Component* set_width(IntProperty* w);
    
    Component* set_width(IntProperty::IntBinding w);
    
    
    Component* set_height(int h);
    
    //Component* set_height(IntProperty* h);
    
    Component* set_height(IntProperty::IntBinding h);
    
    
    Component* set_pos(Component* relative, int x, int y);
    
    //Component* set_pos(Component* relative, IntProperty* x, IntProperty* y);
    
    Component* set_pos(Component* relative, IntProperty::IntBinding x, IntProperty::IntBinding y);
    
    Component* set_padding(unsigned int top, unsigned int right, unsigned int bottom, unsigned int left);
    
    Component* set_margin(unsigned int top, unsigned int right, unsigned int bottom, unsigned int left);
    
    Component* set_border_enable(bool enabled);
  };
  
}// namespace cydui::components


#define STATE(NAME) \
class NAME##State: public cydui::components::ComponentState { \
public:

#define INIT_STATE(NAME) \
explicit NAME##State(): cydui::components::ComponentState()

#define COMPONENT(NAME) \
class NAME: public cydui::components::Component { \
public: \
logging::logger log = {.name = #NAME, .on = true};

#define DISABLE_LOG this->log.on = false;

#define PROPS(block) \
struct Props         \
block;               \
Props props;

#define NO_PROPS PROPS({})

#define INIT_W_STATE(NAME, STATE_CLASS) \
explicit NAME##Component(STATE_CLASS* state) \
  : cydui::components::Component(state)

#define INIT(NAME) \
explicit NAME(NAME##State* state, Props props, const std::function<void(cydui::components::Component*)>& inner) \
  : cydui::components::Component(state, inner) {                                                                       \
    this->props = std::move(props);

#define REDRAW(EV) \
void on_redraw(cydui::events::layout::CLayoutEvent* (EV)) override

#define RENDER(EV) \
void on_render(cydui::events::layout::CLayoutEvent* (EV)) override


#define WITH_STATE(NAME) auto* state = (NAME##State*)this->state;

#define ADD_TO(COMPONENT, VECTOR) COMPONENT->add(std::vector<cydui::components::Component*>VECTOR);

#define REDRAWw(EV, NAME, VECTOR) \
void on_redraw(cydui::events::layout::CLayoutEvent* (EV)) override { \
WITH_STATE(NAME)         \
ADD_TO(this, VECTOR);    \
}

/// Using lambda captures in statements prevents IDE from showing 'unused-lambda-capture' error

// FIXME - REMOVE MACRO
#define in(NAME, LOCAL_NAME, VECTOR) \
[state](cydui::components::Component* __raw_##LOCAL_NAME) { \
state;                                     \
auto* LOCAL_NAME = (NAME*)__raw_##LOCAL_NAME; \
LOCAL_NAME->add(VECTOR);                    \
}

#define C_NEW_ALL(ID, NAME, _PROPS, IN, INIT) \
new NAME(                                     \
  state->children.contains(ID)?               \
      ((NAME##State*)state->children[ID])     \
    : ((NAME##State*)state->children.add(ID, new NAME##State())), \
  NAME::Props _PROPS,                         \
  [this, state](cydui::components::Component* __raw_local_##NAME) { \
    this; state;                                          \
    auto* this##NAME = (NAME*)__raw_local_##NAME;                 \
                                              \
    std::vector<cydui::components::Component*> v IN;              \
    this##NAME->add(v);                       \
                                              \
    std::function<void(NAME* this##NAME)> init =                  \
    [this,state](NAME* this##NAME){this;state; INIT};            \
    init(this##NAME);                         \
                                              \
  }\
)

#define C_NEW_INNER(ID, NAME, PROPS, IN) C_NEW_ALL(ID, NAME, PROPS, IN, { state; })

#define C_NEW_PROPS(ID, NAME, PROPS) C_NEW_INNER(ID, NAME, PROPS, ({ }))

#define C_NEW(ID, NAME) C_NEW_PROPS(ID, NAME, ({ }))

#define C_GET_NEW_MACRO(_1, _2, _3, _4, NAME, ...) NAME
#define N(...) C_GET_NEW_MACRO(__VA_ARGS__, C_NEW_ALL, C_NEW_INNER, C_NEW_PROPS, C_NEW)(__COUNTER__, __VA_ARGS__)

#endif//CYD_UI_COMPONENTS_HPP
