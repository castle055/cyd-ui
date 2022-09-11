//
// Created by castle on 8/22/22.
//

#include "../../../include/components.hpp"

#include <utility>

#include "../../../include/window_types.hpp"
#include "../../graphics/graphics.hpp"

using namespace cydui::components;

ComponentState::ComponentState() {
  geom.x.bind(this);
  geom.y.bind(this);
  geom.x_off.bind(this);
  geom.y_off.bind(this);
  geom.w.bind(this);
  geom.h.bind(this);
  geom.min_w.bind(this);
  geom.min_h.bind(this);
  geom.padding_top.bind(this);
  geom.padding_right.bind(this);
  geom.padding_bottom.bind(this);
  geom.padding_left.bind(this);
  geom.margin_top.bind(this);
  geom.margin_right.bind(this);
  geom.margin_bottom.bind(this);
  geom.margin_left.bind(this);
}

void ComponentState::dirty() {
  _dirty = true;
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
  
  if (state->stateless_comp)
    delete state;
}

//== API for subclasses
void Component::add(std::vector<Component*> children) {
  for (auto &item: children) {
    item->parent = this;
    this->children.push_back(item);
    
    if (!item->state->geom.custom_offset) {
      item->set_pos(this, 0, 0);
    }
  }
  
  std::vector<Property*> w_deps = { };
  std::vector<Property*> h_deps = { };
  for (const auto        &item: this->children) {
    w_deps.push_back(item->state->geom.abs_w().unwrap());
    w_deps.push_back(&item->state->geom.x);
    h_deps.push_back(item->state->geom.abs_h().unwrap());
    h_deps.push_back(&item->state->geom.y);
  }
  
  if (!state->geom.custom_width) {
    state->geom.w.set_binding(
      [this]() {
        int             max = 0;
        for (const auto &item: this->children) {
          int item_w = item->state->geom.x.val() + item->state->geom.abs_w().compute();
          if (item_w > max)
            max = item_w;
        }
        return max;
      }, w_deps
    );
  }
  if (!state->geom.custom_height) {
    state->geom.h.set_binding(
      [this]() {
        int             max = 0;
        for (const auto &item: this->children) {
          int item_h = item->state->geom.y.val() + item->state->geom.abs_h().compute();
          if (item_h > max)
            max      = item_h;
        }
        return max;
      }, h_deps
    );
  }
}

//== Events
void Component::on_event(events::layout::CLayoutEvent* ev) {
  auto* win_ref = ((window::CWindow*)ev->win)->win_ref;
  switch (ev->type) {
    case events::layout::LYT_EV_REDRAW: redraw(ev, true);
      break;
    case events::layout::LYT_EV_KEYPRESS: break;
    case events::layout::LYT_EV_KEYRELEASE: break;
    case events::layout::LYT_EV_BUTTONPRESS:on_mouse_click(ev);
      break;
    case events::layout::LYT_EV_BUTTONRELEASE: break;
    case events::layout::LYT_EV_RESIZE:state->geom.w = win_ref->w;
      state->geom.h                                  = win_ref->h;
      ev->consumed                                   = true;
      
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
  if (!parent) {
    ev->consumed = true;
  }
}

void Component::redraw(cydui::events::layout::CLayoutEvent* ev, bool clr) {
  auto* win_ref = ((window::CWindow*)ev->win)->win_ref;
  
  if (clr) {
    // Clear window region
    graphics::clr_rect(
      win_ref,
      state->geom.abs_x().compute(),
      state->geom.abs_y().compute(),
      state->geom.abs_w().compute(),
      state->geom.abs_h().compute());
  }
  for (auto &child: children)
    delete child;
  children.clear();
  
  inner_redraw(this);
  on_redraw(ev);
  
  for (auto &child: children) {
    child->redraw(ev, false);
  }
  
  if (state->border.enabled) {
    graphics::drw_rect(
      win_ref,
      state->border.color,
      state->geom.border_x().compute(),
      state->geom.border_y().compute(),
      state->geom.border_w().compute(),
      state->geom.border_h().compute(),
      false
    );
  }
  
  
  if (clr) {
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

Component* Component::set_size(int w, int h) {
  state->geom.set_size(w, h);
  return this;
}

Component* Component::set_size(IntProperty* w, IntProperty* h) {
  state->geom.set_size(w, h);
  return this;
}

Component* Component::set_size(IntProperty::IntBinding w, IntProperty::IntBinding h) {
  state->geom.set_size(w, h);
  return this;
}


Component* Component::set_width(int w) {
  state->geom.set_width(w);
  return this;
}

//Component* Component::set_width(IntProperty* w) {
//  state->geom.set_width(w);
//  return this;
//}

Component* Component::set_width(IntProperty::IntBinding w) {
  state->geom.set_width(w);
  return this;
}


Component* Component::set_height(int h) {
  state->geom.set_height(h);
  return this;
}

//Component* Component::set_height(IntProperty* h) {
//  state->geom.set_height(h);
//  return this;
//}
//
Component* Component::set_height(IntProperty::IntBinding h) {
  state->geom.set_height(h);
  return this;
}


Component* Component::set_pos(Component* relative, int x, int y) {
  state->geom.set_pos(&relative->state->geom, x, y);
  return this;
}

//Component* Component::set_pos(Component* relative, IntProperty* x, IntProperty* y) {
//  state->geom.set_pos(&relative->state->geom, x, y);
//  return this;
//}
//
Component* Component::set_pos(Component* relative, IntProperty::IntBinding x, IntProperty::IntBinding y) {
  state->geom.set_pos(&relative->state->geom, x, y);
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
