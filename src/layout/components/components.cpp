//
// Created by castle on 8/22/22.
//

#include "../../../include/components.h"

#include <utility>

#include "../../../include/window_types.h"
#include "../../graphics/graphics.h"

using namespace cydui::components;

void ComponentState::dirty() {
  events::emit(
      new cydui::events::CEvent {
          .type      = cydui::events::EVENT_LAYOUT,
          .raw_event = nullptr,
          .data      = new events::layout::CLayoutEvent {
              .type = events::layout::LYT_EV_REDRAW,
              .data = events::layout::CLayoutData {
                  .redraw_ev = events::layout::CRedrawEvent {
                      .x = 0,
                      .y = 0,
                      .component = this,
                  }
              },
          }
      }
  );
}

//===== COMPONENTS
//== Constructors
//Component::Component(std::unordered_set<Component*> children)
//    : Component(new ComponentState(), children) {
//  state->stateless_comp = true;
//}

//Component::Component(ComponentState* state, std::unordered_set<Component*> children)
//    : state(state) {
//  state->component_instance = this;
//  addParamChildren(children);
//  parent = nullptr;
//}

Component::Component()
    : Component([](Component*) { }) {
}

Component::Component(std::function<void(Component*)> inner)
    : Component(new ComponentState(), inner) {
  state->stateless_comp = true;
}

Component::Component(ComponentState* state)
    : Component(state, [](Component*) { }) {
}

Component::Component(ComponentState* state, std::function<void(Component*)> inner)
    : state(state) {
  state->component_instance = this;
  this->inner_redraw        = inner;
  parent = nullptr;
}

//== Destructor
Component::~Component() {
  for (auto &child: children)
    delete child;
  children.clear();
  //for (auto &child: param_children)
  //  delete child;
  //param_children.clear();
  state->component_instance = nullptr;
}

//== API for subclasses
void Component::add(std::vector<Component*> children) {
  for (auto &item: children) {
    item->parent = this;
    this->children.push_back(item);
    
    if (!item->state->geom.custom_offset) {
      item->state->geom.x_off       = state->geom.content_x();
      item->state->geom.y_off       = state->geom.content_y();
      item->state->geom.relative_to = this;
    }
  }
}

//== Events
void Component::on_event(events::layout::CLayoutEvent* ev) {
  auto* win_ref = ((window::CWindow*)ev->win)->win_ref;
  bool dirty = false;
  switch (ev->type) {
    case events::layout::LYT_EV_REDRAW: redraw(ev, true);
      break;
    case events::layout::LYT_EV_KEYPRESS: break;
    case events::layout::LYT_EV_KEYRELEASE: break;
    case events::layout::LYT_EV_BUTTONPRESS:on_mouse_click(ev);
      break;
    case events::layout::LYT_EV_BUTTONRELEASE: break;
    case events::layout::LYT_EV_RESIZE:
      if (state->geom.w != win_ref->w || state->geom.h != win_ref->h)
        dirty = true;
      state->geom.w = win_ref->w;
      state->geom.h = win_ref->h;
      ev->consumed  = true;
      if (dirty)
        state->dirty();
      break;
    case events::layout::LYT_EV_MOUSEMOTION:
      if (ev->data.motion_ev.enter) {
        on_mouse_enter(ev);
      } else if (ev->data.motion_ev.exit) {
        on_mouse_exit(ev);
      }
      break;
    default: break;
  }
  
  if (parent && !ev->consumed) {
    parent->on_event(ev);
  }
}

void Component::redraw(cydui::events::layout::CLayoutEvent* ev, bool clr) {
  auto* win_ref = ((window::CWindow*)ev->win)->win_ref;
  
  if (clr) {
    // Clear window region
    graphics::clr_rect(win_ref, state->geom.abs_x(), state->geom.abs_y(), state->geom.abs_w(), state->geom.abs_h());
  }
  for (auto &child: children)
    delete child;
  children.clear();
  
  if (state->border.enabled) {
    graphics::drw_rect(
        win_ref,
        state->border.color,
        state->geom.border_x(),
        state->geom.border_y(),
        state->geom.border_w(),
        state->geom.border_h(),
        false
    );
  }
  
  inner_redraw(this);
  on_redraw(ev);
  
  for (auto &child: children) {
    child->redraw(ev, false);
  }
  
  if (clr) {
    graphics::flush(win_ref);
    ev->consumed = true;
  }
  
  state->_dirty = false;
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

Component* Component::set_min_size(int w, int h) {
  state->geom.min_w =
      w - state->geom.padding_left - state->geom.padding_right - state->geom.margin_left - state->geom.margin_right;
  state->geom.min_h =
      h - state->geom.padding_top - state->geom.padding_bottom - state->geom.margin_top - state->geom.margin_bottom;
  
  if (state->geom.w < state->geom.min_w)
    state->geom.w = state->geom.min_w;
  if (state->geom.h < state->geom.min_h)
    state->geom.h = state->geom.min_h;
  
  return this;
}

Component* Component::set_pref_size(int w, int h) {
  state->geom.w =
      w - state->geom.padding_left - state->geom.padding_right - state->geom.margin_left - state->geom.margin_right;
  state->geom.h =
      h - state->geom.padding_top - state->geom.padding_bottom - state->geom.margin_top - state->geom.margin_bottom;
  if (state->geom.w < state->geom.min_w)
    state->geom.w = state->geom.min_w;
  if (state->geom.h < state->geom.min_h)
    state->geom.h = state->geom.min_h;
  return this;
}

Component* Component::set_pos(Component* relative, int x, int y) {
  if (relative) {
    state->geom.x_off       = relative->state->geom.content_x() + x;
    state->geom.y_off       = relative->state->geom.content_y() + y;
    state->geom.relative_to = relative;
  } else {
    state->geom.x_off       = x;
    state->geom.y_off       = y;
    state->geom.relative_to = nullptr;
  }
  state->geom.custom_offset = true;
  return this;
}

Component* Component::set_padding(unsigned int top, unsigned int right, unsigned int bottom, unsigned int left) {
  state->geom.padding_top    = top;
  state->geom.padding_right  = right;
  state->geom.padding_bottom = bottom;
  state->geom.padding_left   = left;
  return this;
}

Component* Component::set_margin(unsigned int top, unsigned int right, unsigned int bottom, unsigned int left) {
  state->geom.margin_top    = top;
  state->geom.margin_right  = right;
  state->geom.margin_bottom = bottom;
  state->geom.margin_left   = left;
  return this;
}

Component* Component::set_border_enable(bool enabled) {
  state->border.enabled = enabled;
  return this;
}

Component* Component::get_parent() {
  return parent;
}

int ComponentGeometry::abs_x() const {
  return x + x_off;
}

int ComponentGeometry::abs_y() const {
  return y + y_off;
}

int ComponentGeometry::content_x() const {
  return x + x_off + (int)margin_left + (int)padding_left;
}

int ComponentGeometry::content_y() const {
  return y + y_off + (int)margin_top + (int)padding_top;
}

int ComponentGeometry::border_x() const {
  return x + x_off + (int)margin_left;
}

int ComponentGeometry::border_y() const {
  return y + y_off + (int)margin_top;
}


int ComponentGeometry::abs_w() const {
  return w + (int)padding_left + (int)padding_right + (int)margin_left + (int)margin_right;
}

int ComponentGeometry::abs_h() const {
  return h + (int)padding_top + (int)padding_bottom + (int)margin_top + (int)margin_bottom;
}

int ComponentGeometry::content_w() const {
  return w;
}

int ComponentGeometry::content_h() const {
  return h;
}

int ComponentGeometry::border_w() const {
  return w + (int)padding_left + (int)padding_right;
}

int ComponentGeometry::border_h() const {
  return h + (int)padding_top + (int)padding_bottom;
}
