//
// Created by castle on 8/21/22.
//

#ifndef CYD_UI_COMPONENTS_H
#define CYD_UI_COMPONENTS_H

#include "../src/events/events.h"
#include <vector>


#define PROPS(P, A)                                                            \
  this, new P##Props(this, [](Component* parent, P##Props* p) { A })

#define STATE(COMP) ((COMP##State*)parent->state)

namespace cydui::components {
  class Component;

  class ComponentState {
  public:
    int x = 0, y = 0, w = 0, h = 0;
    bool focused = false;
  };

  class Component {

    Component* parent = nullptr;

    void redraw(cydui::events::layout::CLayoutEvent* ev, bool clr);

    virtual void on_redraw(events::layout::CLayoutEvent* ev);
    virtual void on_mouse_enter(events::layout::CLayoutEvent* ev);
    virtual void on_mouse_exit(events::layout::CLayoutEvent* ev);
    virtual void on_mouse_click(events::layout::CLayoutEvent* ev);
    virtual void on_scroll(events::layout::CLayoutEvent* ev);
    virtual void on_key_press(events::layout::CLayoutEvent* ev);
    virtual void on_key_release(events::layout::CLayoutEvent* ev);

    void addParamChildren(std::vector<Component*> children);

  public:
    explicit Component(std::vector<Component*> children);
    explicit Component(ComponentState* state, std::vector<Component*> children);
    virtual ~Component();
    ComponentState* state;
    void add(std::vector<Component*> children);

    std::vector<Component*> children;
    std::vector<Component*> paramChildren;

    void on_event(events::layout::CLayoutEvent* ev);

    Component* set_pos(int x, int y);
  };
}// namespace cydui::components

#endif//CYD_UI_COMPONENTS_H
