//
// Created by castle on 8/21/22.
//

#include "../../include/layout.hpp"
#include "../logging/logging.hpp"
#include "../events/properties/properties.hpp"

logging::logger log_lay = {.name = "LAYOUT", .on = false};

cydui::layout::Layout::Layout(cydui::components::Component* root): root(root) {
  events::start();
}

void cydui::layout::Layout::bind_window(cydui::window::CWindow* _win) {
  this->win = _win;
}

static void render_if_dirty(cydui::components::Component* c) {
  if (c->state->_dirty) {
    cydui::events::emit(
        new cydui::events::CEvent {
            .type      = cydui::events::EVENT_LAYOUT,
            .raw_event = nullptr,
            .data      = new cydui::events::layout::CLayoutEvent {
                .type = cydui::events::layout::LYT_EV_REDRAW,
                .data = cydui::events::layout::CLayoutData {
                    .redraw_ev = cydui::events::layout::CRedrawEvent {
                        .x = 0,
                        .y = 0,
                        .component = c->state,
                    }
                },
            }
        }
    );
  } else {
    for (auto &item: c->children)
      render_if_dirty(item);
  }
}

void cydui::layout::Layout::on_event(cydui::events::layout::CLayoutEvent* ev) {
  //  log_lay.debug("Event %d", ev->type);
  
  ev->win = (void*)win;
  components::Component     * target       = nullptr;
  components::ComponentState* target_state = nullptr;
  switch (ev->type) {
    case events::layout::LYT_EV_REDRAW:
      log_lay.debug(
          "REDRAW"
      );
      if (ev->data.redraw_ev.component) {
        target_state = ((components::ComponentState*)ev->data.redraw_ev.component);
        if (target_state)
          target     = target_state->component_instance;
        if (target)
          target->on_event(ev);
      } else {
        root->on_event(ev);
      }
      render_if_dirty(root);
      graphics::flush(win->win_ref);
      break;
    case events::layout::LYT_EV_KEYPRESS: break;
    case events::layout::LYT_EV_KEYRELEASE: break;
    case events::layout::LYT_EV_BUTTONPRESS:target = find_by_coords(root, ev->data.button_ev.x, ev->data.button_ev.y);
      if (!target)
        break;
      ev->data.motion_ev.x -= target->state->geom.border_x().compute();
      ev->data.motion_ev.y -= target->state->geom.border_y().compute();
      target->on_event(ev);
      render_if_dirty(root);
      break;
    case events::layout::LYT_EV_BUTTONRELEASE: break;
    case events::layout::LYT_EV_MOUSEMOTION:
      //      log_lay.debug("MOTION x=%d, y=%d", ev->data.motion_ev.x, ev->data.motion_ev.y);
      target = find_by_coords(root, ev->data.motion_ev.x, ev->data.motion_ev.y);
      if (!target)
        break;
      ev->data.motion_ev.x -= target->state->geom.border_x().compute();
      ev->data.motion_ev.y -= target->state->geom.border_y().compute();
      if (focused != target->state) {
        if (focused && focused->component_instance) {
          ev->data.motion_ev.exit = true;
          focused->component_instance->on_event(ev);
          ev->consumed            = false;
          ev->data.motion_ev.exit = false;
        }
        ev->data.motion_ev.enter = true;
        focused = target->state;
      }
      //if (ev->data.motion_ev.enter) {
      //  log_lay.debug(
      //      "MOTION w=%d, h=%d", ev->data.motion_ev.x, ev->data.motion_ev.y
      //  );
      //}
      target->on_event(ev);
      render_if_dirty(root);
      break;
    case events::layout::LYT_EV_RESIZE:
      log_lay.debug(
          "RESIZE w=%d, h=%d", ev->data.resize_ev.w, ev->data.resize_ev.h
      );
      root->on_event(ev);
      render_if_dirty(root);
      break;
    case events::layout::LYT_EV_UPDATE_PROP:
      ((Property*)ev->data.update_prop_ev.target_property)
          ->set_raw_value((void*)(ev->data.update_prop_ev.new_value));
      render_if_dirty(root);
      break;
    default: break;
  }
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
#pragma ide diagnostic ignored "modernize-loop-convert"

cydui::components::Component* cydui::layout::Layout::find_by_coords(components::Component* c, int x, int y) {
  components::Component* target = nullptr;
  for (auto i = c->children.rbegin(); i != c->children.rend(); ++i) {
    auto* item = *i;
    if (x >= item->state->geom.border_x().compute()
        && x < (item->state->geom.border_x().compute() + item->state->geom.border_w().compute())
        && y >= item->state->geom.border_y().compute()
        && y < (item->state->geom.border_y().compute() + item->state->geom.border_h().compute())
        ) {
      target = find_by_coords(item, x, y);
      if (target)
        break;
    }
  }
  if (target)
    return target;
  
  if (!c->state->stateless_comp && x >= c->state->geom.border_x().compute()
      && x < (c->state->geom.border_x().compute() + c->state->geom.border_w().compute())
      && y >= c->state->geom.border_y().compute()
      && y < (c->state->geom.border_y().compute() + c->state->geom.border_h().compute())
      ) {
    target = c;
  }
  return target;
}

#pragma clang diagnostic pop
