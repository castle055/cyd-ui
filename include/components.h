//
// Created by castle on 8/21/22.
//

#ifndef CYD_UI_COMPONENTS_H
#define CYD_UI_COMPONENTS_H

#include "../src/events/events.h"
#include "../src/layout/color/colors.h"
#include <vector>


#define PROPS(P, A)                                                            \
  this, new P##Props(this, [](Component* parent, P##Props* p) { A })

#define STATE(COMP) ((COMP##State*)parent->state)

namespace cydui::components {
  class Component;
  
  class ComponentGeometry {
  public:
    int x     = 0,
        y     = 0,
        x_off = 0,
        y_off = 0,
        min_w = 0,
        min_h = 0,
        w     = 0,
        h     = 0;
    
    unsigned int padding_top    = 0,
                 padding_right  = 0,
                 padding_bottom = 0,
                 padding_left   = 0;
    
    unsigned int margin_top    = 0,
                 margin_right  = 0,
                 margin_bottom = 0,
                 margin_left   = 0;
    
    Component* relative_to = nullptr;
    bool custom_offset   = false;
    bool is_min_size_set = false;
    
    [[nodiscard]] int abs_x() const;
    
    [[nodiscard]] int abs_y() const;
    
    [[nodiscard]] int border_x() const;
    
    [[nodiscard]] int border_y() const;
    
    [[nodiscard]] int content_x() const;
    
    [[nodiscard]] int content_y() const;
    
    [[nodiscard]] int abs_w() const;
    
    [[nodiscard]] int abs_h() const;
    
    [[nodiscard]] int border_w() const;
    
    [[nodiscard]] int border_h() const;
    
    [[nodiscard]] int content_w() const;
    
    [[nodiscard]] int content_h() const;
  };
  
  class ComponentBorder {
  public:
    bool enabled = false;
    layout::color::Color* color = new layout::color::Color("#FCAE1E");
  };
  
  class ComponentState {
  public:
    Component* component_instance;
    //int  x       = 0,
    //     y       = 0,
    //     w       = 0,
    //     h       = 0;
    bool focused = false;
    
    void dirty();
    
    ComponentGeometry geom;
    ComponentBorder   border;
  };
  
  class Component {
    
    Component* parent;
    
    void redraw(cydui::events::layout::CLayoutEvent* ev, bool clr);
    
    virtual void on_redraw(events::layout::CLayoutEvent* ev);
    
    virtual void on_mouse_enter(events::layout::CLayoutEvent* ev);
    
    virtual void on_mouse_exit(events::layout::CLayoutEvent* ev);
    
    virtual void on_mouse_click(events::layout::CLayoutEvent* ev);
    
    virtual void on_scroll(events::layout::CLayoutEvent* ev);
    
    virtual void on_key_press(events::layout::CLayoutEvent* ev);
    
    virtual void on_key_release(events::layout::CLayoutEvent* ev);
    
    void addParamChildren(std::vector<Component*> children);
  
  protected:
    // Private size operations
  public:
    explicit Component(std::vector<Component*> children);
    
    explicit Component(ComponentState* state, std::vector<Component*> children);
    
    virtual ~Component();
    
    ComponentState* state;
    
    void add(std::vector<Component*> children);
    
    std::vector<Component*> children;
    std::vector<Component*> orig_param_children;
    std::vector<Component*> param_children;
    
    void on_event(events::layout::CLayoutEvent* ev);
    
    Component* get_parent();
    
    Component* set_min_size(int w, int h);
    
    Component* set_pref_size(int w, int h);
    
    Component* set_pos(Component* relative, int x, int y);
    
    Component* set_padding(unsigned int top, unsigned int right, unsigned int bottom, unsigned int left);
    
    Component* set_margin(unsigned int top, unsigned int right, unsigned int bottom, unsigned int left);
    
    Component* set_border_enable(bool enabled);
  };
}// namespace cydui::components

#endif//CYD_UI_COMPONENTS_H
