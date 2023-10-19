//
// Created by castle on 8/22/22.
//

#include "core/components.hpp"
#include "core/layout.h"

#include <utility>

using namespace cydui::components;

ComponentState::ComponentState() {
}

void ComponentState::dirty() {
  _dirty = true;
}

//===== COMPONENTS
Component::Component(): Component([](Component*) {
}) {
}

Component::Component(std::function<void(Component*)> inner)
  : Component(new ComponentState(), inner) {
  (*state.unwrap())->stateless_comp = true;
}

Component::Component(ComponentState* state)
  : Component(state, [](Component*) {
}) {
}

Component::Component(
  ComponentState* state, std::function<void(Component*)> inner
)
  : state(state), dim(&state->dim) {
  (*this->state.unwrap())->component_instance = this;
  this->inner_redraw = std::move(inner);
  parent.set_null();
}

//== Destructor
Component::~Component() {
  for (auto &child: children)
    delete child;
  children.clear();
  //for (auto &child: param_children)
  //  delete child;
  //param_children.clear();
  if (state.let(_(ComponentState *, {
    it->component_instance = nullptr;
    return it->stateless_comp;
  })) or false) {
    state.set_null();
  }
}

//== API for subclasses
Component* Component::new_group() {
  auto* c = new Component();
  c->is_group = true;
  return c;
}

void Component::add(
  const std::vector<component_builder_t> &ichildren, bool prepend
) const {
  // TODO - Needs to be recursive when flattening groups, not just first layer
  for (auto &item: ichildren) {
    auto* child = item.build(item);
    if (child == nullptr)
      continue;
    if (child->is_group) {
      for (auto &subitem: child->children) {
        if (subitem == nullptr || !subitem->state)
          continue;
        subitem->parent = (Component*) this;
        (*subitem->state.unwrap())->win = (*state.unwrap())->win;
        if (prepend) {
          this->children.push_front(subitem);
        } else {
          this->children.push_back(subitem);
        }
      }
      child->children.clear();
      delete child;
    } else {
      child->parent = (Component*) this;
      (*child->state.unwrap())->win = (*state.unwrap())->win;
      if (prepend) {
        this->children.push_front(child);
      } else {
        this->children.push_back(child);
      }
    }
  }
}

void Component::redraw(cydui::layout::Layout* layout) {
  inner_redraw(this);
  on_redraw();
  
  for (auto &child: children) {
    child->redraw(layout);
  }
  state.let(_(ComponentState *, {
    it->dragging_context = &layout->dragging_context;
    it->_dirty = false;
  }));
}

void Component::render(cydui::compositing::compositing_node_t* node) const {
  node->id = (unsigned long) (*this->state.unwrap());
  node->op = {
    .x = dim->x.val(),
    .y = dim->y.val(),
    .w = dim->w.val(),
    .h = dim->h.val(),
    .rot = 0.0, // dim->rot.val(),
    .scale_x = 1.0, // dim->scale_x.val(),
    .scale_y = 1.0, // dim->scale_y.val(),
  };
  
  if (is_drawable()) {
    on_render(node->graphics);
  } else {
    for (const auto &child: children) {
      if (child) {
        auto* c_node = new compositing::compositing_node_t();
        node->children.push_back(c_node);
        child->render(c_node);
      }
    }
  }
}

bool cydui::components::Component::is_drawable() const {
  return false;
}

#define COMP_EVENT_HANDLER_IMPL(EV, ARGS) void Component::on_##EV ARGS const
#define RECURSIVE

COMP_EVENT_HANDLER_IMPL(render, (vg::vg_fragment_t & graphics)) {
}

COMP_EVENT_HANDLER_IMPL(redraw, ()) {
}

COMP_EVENT_HANDLER_IMPL(key_press, (KeyData key)) { // NOLINT(misc-no-recursion)
  parent.let(_(Component *, {
    it->on_key_press(key);
  }));
}

COMP_EVENT_HANDLER_IMPL(key_release, (KeyData key)) { // NOLINT(misc-no-recursion)
  parent.let(_(Component *, {
    it->on_key_release(key);
  }));
}

COMP_EVENT_HANDLER_IMPL(mouse_enter, (int x, int y)) { // NOLINT(misc-no-recursion)
  parent.let(_(Component *, {
    auto &p = it;
    p->on_mouse_enter(x + dim->x.val(), y + dim->y.val());
  }));
}

COMP_EVENT_HANDLER_IMPL(mouse_click, (int x, int y, int button)) { // NOLINT(misc-no-recursion)
  parent.let(_(Component *, {
    auto &p = it;
    p->on_mouse_click(x + dim->x.val(), y + dim->y.val(), button);
  }));
}

COMP_EVENT_HANDLER_IMPL(mouse_exit, (int x, int y)) { // NOLINT(misc-no-recursion)
  parent.let(_(Component *, {
    auto &p = it;
    p->on_mouse_exit(x + dim->x.val(), y + dim->y.val());
  }));
}

COMP_EVENT_HANDLER_IMPL(mouse_motion, (int x, int y)) { // NOLINT(misc-no-recursion)
  parent.let(_(Component *, {
    auto &p = it;
    p->on_mouse_motion(x + dim->x.val(), y + dim->y.val());
  }));
}

COMP_EVENT_HANDLER_IMPL(drag_start, (int x, int y)) { // NOLINT(misc-no-recursion)
  parent.let(_(Component *, {
    auto &p = it;
    p->on_drag_start(x + dim->x.val(), y + dim->y.val());
  }));
}

COMP_EVENT_HANDLER_IMPL(drag_motion, (int x, int y)) { // NOLINT(misc-no-recursion)
  parent.let(_(Component *, {
    auto &p = it;
    p->on_drag_motion(x + dim->x.val(), y + dim->y.val());
  }));
}

COMP_EVENT_HANDLER_IMPL(drag_finish, (int x, int y)) { // NOLINT(misc-no-recursion)
  parent.let(_(Component *, {
    auto &p = it;
    p->on_drag_finish(x + dim->x.val(), y + dim->y.val());
  }));
}

COMP_EVENT_HANDLER_IMPL(scroll, (int dx, int dy)) { // NOLINT(misc-no-recursion)
  parent.let(_(Component *, {
    it->on_scroll(dx, dy);
  }));
}

#undef COMP_EVENT_HANDLER_IMPL

nullable<Component*> Component::get_parent() const {
  return parent;
}
