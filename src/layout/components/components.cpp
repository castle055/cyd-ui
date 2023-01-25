//
// Created by castle on 8/22/22.
//

#include "components.hpp"

#include <utility>

#include "../../../include/graphics.hpp"

using namespace cydui::components;

ComponentState::ComponentState() {
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
  : Component([](Component*) {
}) {
}

Component::Component(std::function<void(Component*)> inner)
  : Component(new ComponentState(), inner) {
  state->stateless_comp = true;
}

Component::Component(ComponentState* state)
  : Component(state, [](Component*) {
}) {
}

Component::Component(ComponentState* state, std::function<void(Component*)> inner)
  : state(state), dim(&state->dim) {
  state->component_instance = this;
  this->inner_redraw = std::move(inner);
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
Component* Component::new_group() {
  auto* c = new Component();
  c->is_group = true;
  return c;
}

void Component::add(std::vector<Component*> ichildren) {
  // TODO - Needs to be recursive when flattening groups, not just first layer
  for (auto &item: ichildren) {
    if (item == nullptr) continue;
    if (item->is_group) {
      for (auto &subitem: item->children) {
        if (subitem == nullptr || !subitem->state) continue;
        subitem->parent = this;
        this->children.push_back(subitem);
      }
      item->children.clear();
      delete item;
    } else {
      item->parent = this;
      this->children.push_back(item);
    }
  }
}

void Component::redraw() {
  inner_redraw(this);
  on_redraw();
  
  for (auto &child: children) {
    child->redraw();
  }
  state->_dirty = false;
}

void Component::render(const cydui::window::CWindow* win) {
  auto* win_ref = win->win_ref;
  
  for (auto &child: children) {
    if (child) child->render(win);
  }
  
  if (state != nullptr && state->border.enabled) {
    graphics::drw_rect(
      win_ref,
      state->border.color,
      state->dim.cx.val() - state->dim.padding.left.val(),
      state->dim.cy.val() - state->dim.padding.top.val(),
      state->dim.cw.val() + state->dim.padding.left.val() + state->dim.padding.right.val() + 2,
      state->dim.ch.val() + state->dim.padding.top.val() + state->dim.padding.bottom.val() + 1,
      false
    );
  }
  
  on_render(win);
}

void Component::on_render(const cydui::window::CWindow* win) {
}

void Component::on_redraw() {
}

void Component::on_key_press() {
}

void Component::on_key_release() {
}

void Component::on_mouse_enter(int x, int y) {
  // TODO - Must change coords since they are relative
  if (this->parent)
    this->parent->on_mouse_enter(x, y);
}

void Component::on_mouse_click(int x, int y, int button) {
  // TODO - Must change coords since they are relative
  if (this->parent)
    this->parent->on_mouse_click(x, y, button);
}

void Component::on_mouse_exit(int x, int y) {
  // TODO - Must change coords since they are relative
  if (this->parent)
    this->parent->on_mouse_exit(x, y);
}

void Component::on_scroll(int d) {
  if (this->parent)
    this->parent->on_scroll(d);
}

//Component* Component::set_size(int w, int h) {
//    state->geom.set_size(w, h);
//    return this;
//}
//
//Component* Component::set_size(IntProperty* w, IntProperty* h) {
//    state->geom.set_size(w, h);
//    return this;
//}
//
//Component* Component::set_size(IntProperty::IntBinding w, IntProperty::IntBinding h) {
//    state->geom.set_size(w, h);
//    return this;
//}
//
//
//Component* Component::set_width(int w) {
//    state->geom.set_width(w);
//    return this;
//}
//
///=====================================================================
//Component* Component::set_width(IntProperty* w) {
//  state->geom.set_width(w);
//  return this;
//}

///=====================================================================
//Component* Component::set_width(IntProperty::IntBinding w) {
//    state->geom.set_width(w);
//    return this;
//}
//
//
//Component* Component::set_height(int h) {
//    state->geom.set_height(h);
//    return this;
//}
///=====================================================================

//Component* Component::set_height(IntProperty* h) {
//  state->geom.set_height(h);
//  return this;
//}
//
///=====================================================================
//Component* Component::set_height(IntProperty::IntBinding h) {
//    state->geom.set_height(h);
//    return this;
//}
//
//
//Component* Component::set_pos(Component* relative, int x, int y) {
//    state->geom.set_pos(&relative->state->geom, x, y);
//    return this;
//}
///=====================================================================

//Component* Component::set_pos(Component* relative, IntProperty* x, IntProperty* y) {
//  state->geom.set_pos(&relative->state->geom, x, y);
//  return this;
//}
//
///=====================================================================
//Component* Component::set_pos(Component* relative, IntProperty::IntBinding x, IntProperty::IntBinding y) {
//    state->geom.set_pos(&relative->state->geom, x, y);
//    return this;
//}
//
//Component* Component::set_padding(unsigned int top, unsigned int right, unsigned int bottom, unsigned int left) {
//    state->geom.padding_top = top;
//    state->geom.padding_right = right;
//    state->geom.padding_bottom = bottom;
//    state->geom.padding_left = left;
//    return this;
//}
//
//Component* Component::set_margin(unsigned int top, unsigned int right, unsigned int bottom, unsigned int left) {
//    state->geom.margin_top = top;
//    state->geom.margin_right = right;
//    state->geom.margin_bottom = bottom;
//    state->geom.margin_left = left;
//    return this;
//}
///=====================================================================

Component* Component::set_border_enable(bool enabled) {
  state->border.enabled = enabled;
  return this;
}

Component* Component::get_parent() {
  return parent;
}
