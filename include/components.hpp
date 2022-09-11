//
// Created by castle on 8/21/22.
//

#ifndef CYD_UI_COMPONENTS_HPP
#define CYD_UI_COMPONENTS_HPP

#include "../src/events/events.hpp"
#include "../src/layout/color/colors.hpp"
#include "../src/events/properties/properties.hpp"
#include "../src/layout/components/geometry/component_geometry.hpp"
#include "../src/layout/components/state/children_state_collection.h"
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
    
    Component                       * parent;
    
    std::function<void(Component*)> inner_redraw = nullptr;
    
    void redraw(cydui::events::layout::CLayoutEvent* ev, bool clr);
    
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
    
    ComponentState* state;
    
    void add(std::vector<Component*> children);
    
    std::vector<Component*> children;
    
    void on_event(events::layout::CLayoutEvent* ev);
    
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
public:

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
  : cydui::components::Component(state)

#define REDRAW(EV) \
void on_redraw(cydui::events::layout::CLayoutEvent* (EV)) override


#define WITH_STATE(NAME) auto* state = (NAME##State*)this->state;

#define ADD_TO(COMPONENT, VECTOR) COMPONENT->add(std::vector<cydui::components::Component*>VECTOR);

#define COMMA ,

#define in(NAME, LOCAL_NAME, VECTOR) \
[state](cydui::components::Component* __raw_##LOCAL_NAME) { \
auto* LOCAL_NAME = (NAME*)__raw_##LOCAL_NAME; \
LOCAL_NAME->add(VECTOR);                    \
}

#define C_NEW_ALL(ID, NAME, _PROPS, IN, INIT) \
(new NAME( \
state->children.contains(ID)? \
  ((NAME##State*)state->children[ID]) \
  : ((NAME##State*)state->children.add(ID, new NAME##State())), \
NAME::Props _PROPS,                                   \
[state](cydui::components::Component* __raw_local_##NAME) { \
auto* this##NAME = (NAME*)__raw_local_##NAME; \
this##NAME->add(std::vector<cydui::components::Component*>IN);                         \
                                             \
std::function<void(NAME* this##NAME)> init = [state](NAME* this##NAME)INIT;              \
init(this##NAME);                            \
                                             \
}))

#define C_NEW_INNER(ID, NAME, PROPS, IN) \
C_NEW_ALL(ID, NAME, PROPS, IN, {})

#define C_NEW_PROPS(ID, NAME, PROPS) \
C_NEW_INNER(ID, NAME, PROPS, {})

#define C_NEW(ID, NAME) \
C_NEW_PROPS(ID, NAME, ({}))

#define C_GET_NEW_MACRO(_1, _2, _3, _4, NAME, ...) NAME
#define N(...) C_GET_NEW_MACRO(__VA_ARGS__, C_NEW_ALL, C_NEW_INNER, C_NEW_PROPS, C_NEW)(__COUNTER__, __VA_ARGS__)

#endif//CYD_UI_COMPONENTS_HPP
