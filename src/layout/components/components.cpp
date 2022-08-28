//
// Created by castle on 8/22/22.
//

#include "../../../include/components.h"

#include "../../../include/window_types.h"
#include <utility>

using namespace cydui::components;

//===== COMPONENTS
//== Constructors
Component::Component(std::vector<Component*> children)
    : Component(new ComponentState(), children) {
}

Component::Component(ComponentState* state, std::vector<Component*> children)
    : state(state) {
  addParamChildren(children);
}

//== Destructor
Component::~Component() {
  for (auto& child: children)
    delete child;
  children.clear();
  for (auto& child: paramChildren)
    delete child;
  paramChildren.clear();
}

//== API for subclasses
void Component::add(std::vector<Component*> children) {
  for (auto& item: children) {
    item->parent = this;
    this->children.push_back(item);
  }
}

void Component::addParamChildren(std::vector<Component*> children) {
  for (auto& item: children) {
    item->parent = this;
    this->paramChildren.push_back(item);
  }
}

//== Events
void Component::on_event(events::layout::CLayoutEvent* ev) {
  switch (ev->type) {
    case events::layout::LYT_EV_REDRAW: redraw(ev, true); break;
    case events::layout::LYT_EV_KEYPRESS: break;
    case events::layout::LYT_EV_KEYRELEASE: break;
    case events::layout::LYT_EV_BUTTONPRESS: break;
    case events::layout::LYT_EV_BUTTONRELEASE: break;
    case events::layout::LYT_EV_RESIZE:
      state->w = ev->data.resize_ev.w;
      state->h = ev->data.resize_ev.h;
      break;
    default: break;
  }
}

void Component::redraw(cydui::events::layout::CLayoutEvent* ev, bool clr) {
  if (clr) {
    // Clear window region
    auto* win_ref = ((window::CWindow*)ev->win)->win_ref;
    graphics::clr_rect(win_ref, state->x, state->y, state->w + 1, state->h + 1);
  }

  for (auto& child: children)
    delete child;
  children.clear();

  on_redraw(ev);

  for (auto& child: children)
    child->redraw(ev, false);
  for (auto& child: paramChildren)
    child->redraw(ev, false);
}

void Component::on_redraw(events::layout::CLayoutEvent* ev) {
}
void Component::on_key_press(events::layout::CLayoutEvent* ev) {
}
void Component::on_key_release(events::layout::CLayoutEvent* ev) {
}
void Component::on_mouse_enter(events::layout::CLayoutEvent* ev) {
}
void Component::on_mouse_click(events::layout::CLayoutEvent* ev) {
}
void Component::on_mouse_exit(events::layout::CLayoutEvent* ev) {
}
void Component::on_scroll(events::layout::CLayoutEvent* ev) {
}

Component* Component::set_pos(int x, int y) {
  state->x = x;
  state->y = y;
  return this;
}
