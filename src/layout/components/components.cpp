//
// Created by castle on 8/22/22.
//

#include "components.hpp"

#include <utility>

#include "../../../include/graphics.hpp"
#include "event_types.h"

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
      if (prepend) {
        this->children.push_front(child);
      } else {
        this->children.push_back(child);
      }
    }
  }
}

void Component::redraw() {
  inner_redraw(this);
  on_redraw();
  
  for (auto &child: children) {
    child->redraw();
  }
  state.let(_(ComponentState *, {
    it->_dirty = false;
  }));
}

void Component::render(cydui::graphics::render_target_t* target) const {
  
  auto* sub_render_target = target;
  state.let(_(ComponentState *, {
    it->sub_render_target.let(_(cydui::graphics::render_target_t * , {
      sub_render_target = it;
      cydui::graphics::clr_rect(sub_render_target, 0, 0, sub_render_target->w, sub_render_target->h);
    }));
  }));
  
  for (const auto &child: children) {
    if (child) {
      child->state.let(_(ComponentState *, {
        if (it->dim.cx.val() + it->dim.cw.val() + it->dim.padding.right.val() > sub_render_target->w
          || it->dim.cy.val() + it->dim.ch.val() + it->dim.padding.bottom.val() > sub_render_target->h) {
          sub_render_target->resize(
            std::max(sub_render_target->w,
              it->dim.cx.val() + it->dim.cw.val() + it->dim.padding.right.val()),
            std::max(sub_render_target->h,
              it->dim.cy.val() + it->dim.ch.val() + it->dim.padding.bottom.val())
          );
        }
      }));
      child->render(sub_render_target);
    }
  }
  
  state.let(_(ComponentState *, {
    if (it->border.enabled) {
      graphics::drw_rect(target,
        it->border.color,
        it->dim.cx.val() - it->dim.padding.left.val(),
        it->dim.cy.val() - it->dim.padding.top.val(),
        it->dim.cw.val() + it->dim.padding.left.val()
          + it->dim.padding.right.val() + 2,
        it->dim.ch.val() + it->dim.padding.top.val()
          + it->dim.padding.bottom.val() + 1,
        false);
    }
  }));
  
  on_render(target);
}

#define COMP_EVENT_HANDLER_IMPL(EV, ARGS) void Component::on_##EV ARGS const
#define RECURSIVE

COMP_EVENT_HANDLER_IMPL(render, (cydui::graphics::render_target_t * target)) {
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

COMP_EVENT_HANDLER_IMPL(scroll, (int dx, int dy)) { // NOLINT(misc-no-recursion)
  parent.let(_(Component *, {
    it->on_scroll(dx, dy);
  }));
}

#undef COMP_EVENT_HANDLER_IMPL

Component* Component::set_border_enable(bool enabled) {
  state.let(_(ComponentState *, {
    it->border.enabled = enabled;
  }));
  return this;
}

nullable<Component*> Component::get_parent() const {
  return parent;
}
