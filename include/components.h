//
// Created by castle on 8/21/22.
//

#ifndef CYD_UI_COMPONENTS_H
#define CYD_UI_COMPONENTS_H

#include "../src/events/events.h"
#include "../src/layout/color/colors.h"
#include "../src/events/properties/properties.h"
#include "../src/layout/components/geometry/component_geometry.h"
#include <vector>
#include <unordered_set>
#include <functional>


#define PROPS(P, A)                                                            \
  this, new P##Props(this, [](Component* parent, P##Props* p) { A })

#define STATE(COMP) ((COMP##State*)parent->state)

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
    
    ComponentGeometry geom;
    ComponentBorder   border;
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
    
    Component* set_pos(Component* relative, int x, int y);
    
    Component* set_pos(Component* relative, IntProperty* x, IntProperty* y);
    
    Component* set_pos(Component* relative, IntProperty::IntBinding x, IntProperty::IntBinding y);
    
    Component* set_padding(unsigned int top, unsigned int right, unsigned int bottom, unsigned int left);
    
    Component* set_margin(unsigned int top, unsigned int right, unsigned int bottom, unsigned int left);
    
    Component* set_border_enable(bool enabled);
  };
  
}// namespace cydui::components

#endif//CYD_UI_COMPONENTS_H
