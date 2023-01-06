//
// Created by castle on 8/21/22.
//

#include "../../include/layout.hpp"
#include "../../include/logging.hpp"
#include "../../include/properties.hpp"
#include "../graphics/events.hpp"

logging::logger log_lay = {.name = "LAYOUT", .on = true, .min_level = logging::INFO};

cydui::layout::Layout::Layout(cydui::components::Component* root): root(root) {
}

static void redraw_component(const cydui::window::CWindow* win, cydui::components::Component* target) {
  log_lay.debug("REDRAW");
  // Clear render area of component instances
  for (auto child: target->children)
    delete child;
  target->children.clear();
  
  // Recreate those instances with redraw(), this set all size hints relationships
  target->redraw();
  
  // Clear screen area
  cydui::graphics::clr_rect(
    win->win_ref,
    target->state->geom.abs_x().compute(),
    target->state->geom.abs_y().compute(),
    target->state->geom.abs_w().compute(),
    target->state->geom.abs_h().compute());
  
  // Render screen area & flush graphics
  target->render(win);
  
  //    if (render_if_dirty(root))
  cydui::graphics::flush(win->win_ref);
}

void cydui::layout::Layout::bind_window(cydui::window::CWindow* _win) {
  this->win = _win;
  
  listen(RedrawEvent, {
    if (it.data->win != 0 && it.data->win != win->win_ref->xwin) return;
    cydui::components::Component* target = root;
    if (it.data->component) {
      cydui::components::ComponentState* target_state     = ((cydui::components::ComponentState*)it.data->component);
      cydui::components::Component     * specified_target = target_state->component_instance;
      if (specified_target)
        target = specified_target;
    }
    
    redraw_component(this->win, target);
  })
  listen(KeyEvent, {
    if (it.data->win != win->win_ref->xwin) return;
    
  })
  listen(ButtonEvent, {
    if (it.data->win != win->win_ref->xwin) return;
    if (it.data->pressed) {
      cydui::components::Component* target           = root;
      cydui::components::Component* specified_target = find_by_coords(root, it.data->x, it.data->y);
      if (specified_target)
        target = specified_target;
      
      int rel_x = it.data->x - target->state->geom.border_x().compute();
      int rel_y = it.data->y - target->state->geom.border_y().compute();
      
      target->on_mouse_click(rel_x, rel_y, it.data->button);
      if (render_if_dirty(root))
        graphics::flush(win->win_ref);
    }
  })
  listen(ScrollEvent, {
    if (it.data->win != win->win_ref->xwin) return;
    cydui::components::Component* target           = root;
    cydui::components::Component* specified_target = find_by_coords(root, it.data->x, it.data->y);
    if (specified_target)
      target = specified_target;
    
    target->on_scroll(it.data->d);
    if (render_if_dirty(root))
      graphics::flush(win->win_ref);
  })
  listen(MotionEvent, {
    if (it.data->win != win->win_ref->xwin) return;
    cydui::components::Component* target           = root;
    cydui::components::Component* specified_target = find_by_coords(root, it.data->x, it.data->y);
    if (specified_target)
      target = specified_target;
    
    if (focused != target->state) {
      if (focused && focused->component_instance) {
        int exit_rel_x = it.data->x - focused->geom.border_x().compute();
        int exit_rel_y = it.data->y - focused->geom.border_y().compute();
        focused->component_instance->on_mouse_exit(exit_rel_x, exit_rel_y);
      }
      focused = target->state;
    }
    
    int rel_x = it.data->x - target->state->geom.border_x().compute();
    int rel_y = it.data->y - target->state->geom.border_y().compute();
    target->on_mouse_enter(rel_x, rel_y);
    
    if (render_if_dirty(root))
      graphics::flush(win->win_ref);
  })
  listen(ResizeEvent, {
    if (it.data->win != win->win_ref->xwin) return;
    log_lay.debug("RESIZE w=%d, h=%d", it.data->w, it.data->h);
    
    root->state->geom.w = win->win_ref->w;
    root->state->geom.h = win->win_ref->h;
    
    if (render_if_dirty(root))
      graphics::flush(win->win_ref);
  })
  listen(UpdatePropEvent, {
    if (it.data->win != win->win_ref->xwin) return;
    ((Property*)it.data->target_property)
      ->set_raw_value((void*)(it.data->new_value));
    if (render_if_dirty(root))
      graphics::flush(win->win_ref);
  })
  
}

bool cydui::layout::Layout::render_if_dirty(cydui::components::Component* c) {
  if (c->state->_dirty) {
    redraw_component(this->win, c);
    return true;
  } else {
    bool      any = false;
    for (auto &item: c->children)
      any = render_if_dirty(item) || any; // F**K, order here matters
    return any;
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
