// Copyright (c) 2024, Victor Castillo, All rights reserved.

//
// Created by castle on 8/21/22.
//

#include "layout.h"
#include <ranges>
#include <algorithm>

logging::logger log_lay = {
  .name = "LAYOUT", .on = true, .min_level = logging::INFO};

#define COMPUTE(DIM)                                                           \
  if (!dimension_t::compute(DIM)) {                                            \
    return false;                                                              \
  }

using namespace cyd::ui;
using namespace cyd::ui::components;

static bool compute_dimensions(component_base_t* rt) {
  using namespace cyd::ui::dimensions;
  component_dimensional_relations_t dim = rt->get_dimensional_relations();
  
  /// COMPUTE SOME VALUES
  COMPUTE(dim.x)
  COMPUTE(dim.y)
  
  COMPUTE(dim.margin_top)
  COMPUTE(dim.margin_right)
  COMPUTE(dim.margin_bottom)
  COMPUTE(dim.margin_left)
  
  COMPUTE(dim.padding_top)
  COMPUTE(dim.padding_right)
  COMPUTE(dim.padding_bottom)
  COMPUTE(dim.padding_left)
  
  /// COMPUTE CHILDREN ORIGIN POINT (cx, cy)
  if (rt->parent.has_value()) {
    component_dimensional_relations_t parent_dim = rt->parent.value()->get_dimensional_relations();
    dim.cx = parent_dim.cx.val()
      + dim.x.val()
      + dim.margin_left.val()
      + dim.padding_left.val();
    dim.cy = parent_dim.cy.val()
      + dim.y.val()
      + dim.margin_top.val()
      + dim.padding_top.val();
  } else {
    dim.cx = dim.x.val() + dim.margin_left.val() + dim.padding_left.val();
    dim.cy = dim.y.val() + dim.margin_top.val() + dim.padding_top.val();
  }
  
  /// COMPUTE SIZE
  if (!dim.fixed_w) {
    dim.w.unknown = true;
  } else {
    COMPUTE(dim.w)
    dim.cw = dim.w.val() - dim.padding_left.val() - dim.padding_right.val()
      - dim.margin_left.val() - dim.margin_right.val();
  }
  if (!dim.fixed_h) {
    dim.h.unknown = true;
  } else {
    COMPUTE(dim.h)
    dim.ch = dim.h.val() - dim.padding_top.val() - dim.padding_bottom.val()
      - dim.margin_top.val() - dim.margin_bottom.val();
  }
  
  /// COMPUTE DIMENSIONS FOR CHILDREN RECURSIVELY
  std::vector<component_base_t*> pending;
  
  dimension_value_t total_w = 0;
  dimension_value_t total_h = 0;
  for (auto &child: rt->children) {
    //compute_dimensions(child);
    // if error (circular dependency), skip for now, and then recalculate
    if (compute_dimensions(child)) {
      component_dimensional_relations_t c_dim = child->get_dimensional_relations();
      dimension_value_t child_max_w = c_dim.x.val() + c_dim.w.val();
      dimension_value_t child_max_h = c_dim.y.val() + c_dim.h.val();
      total_w = std::max(total_w, child_max_w);
      total_h = std::max(total_h, child_max_h);
    } else {
      pending.push_back(child);
    }
  }
  
  if (!dim.fixed_w) {// If not given, or given has error (ie: circular dep)
    dim.cw = total_w;
    dim.w = dim.cw.val() + dim.padding_left.val() + dim.padding_right.val()
      + dim.margin_left.val() + dim.margin_right.val();
  }
  
  if (!dim.fixed_h) {// If not given, or given has error (ie: circular dep)
    dim.ch = total_h;
    dim.h = dim.ch.val() + dim.padding_top.val() + dim.padding_bottom.val()
      + dim.margin_top.val() + dim.margin_bottom.val();
  }
  
  return std::all_of(pending.begin(), pending.end(), compute_dimensions);
}

#undef COMPUTE

void cyd::ui::layout::Layout::recompute_dimensions(
  component_base_t* start_from
) {
  if (!compute_dimensions(start_from) && start_from->parent.has_value()) {
    component_base_t* c = start_from->parent.value();
    while (c && !compute_dimensions(c)) {
      if (!c->parent.has_value()) {
        log_lay.error("Could not compute dimensions");
        // TODO - Catch dimensional error
      }
      c = c->parent.value();
    }
  }
}

void cyd::ui::layout::Layout::redraw_component(component_base_t* target) {
  log_lay.debug("REDRAW");
  //auto t0 = std::chrono::system_clock::now();
  // Clear render area of component instances
  auto* compositing_tree = new compositing::compositing_tree_t;
  
  // TODO - For now the entire screen is redraw everytime, in the future it
  // would be interesting to implement a diff algorithm that could redraw
  // subsections of the screen.
  target->clear_children();
  // Recreate those instances with redraw(), this set all size hints relationships
  target->redraw();
  
  recompute_dimensions(root);
  
  root->get_fragment(this, compositing_tree->root);
  //compositing_tree->fix_dimensions();
  
  compositor.compose(compositing_tree);
  //auto t1 = std::chrono::system_clock::now();
  //printf("redraw time: %ld us\n", std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count());
}

bool cyd::ui::layout::Layout::render_if_dirty(component_base_t* c) {
  if (c->state.value()->_dirty) {
    redraw_component(c);
    return true;
  } else {
    bool any = false;
    for (auto &item: c->children)
      any = render_if_dirty(item) || any;// ! F**K, order here matters
    // ? render_if_dirty() needs to be called before `any` is checked.
    return any;
  }
}

component_base_t* cyd::ui::layout::Layout::find_by_coords(int x, int y) {
  return root->find_by_coords(x, y);
}

//static event_handler_t* get_instance_ev_handler(component_state_t* component_state) {
//
//}

#define INSTANCE_EV_HANDLER(STATE_PTR) \
  if (STATE_PTR->component_instance.has_value()) \
    STATE_PTR->component_instance.value()->event_handler()


void cyd::ui::layout::Layout::bind_window(cyd::ui::window::CWindow* _win) {
  this->win = _win;
  compositor.set_render_target(this->win->win_ref, &this->win->profiling_ctx);
  {/// Configure root component
    root_state->window = this->win;
    
    auto dim = root->get_dimensional_relations();
    dim.w = (int) get_frame(this->win->win_ref)->width();
    dim.h = (int) get_frame(this->win->win_ref)->height();
    dim.fixed_w = true;
    dim.fixed_h = true;
    root->configure_event_handler();
    root->subscribe_events();
  }
  
  listeners.push_back(win->listen(RedrawEvent, {
    auto _pev = this->win->profiling_ctx.scope_event("Redraw");
    if (ev.data->component) {
      component_state_t* target_state =
        ((component_state_t*) ev.data->component);
      if (target_state->component_instance.has_value()) {
        redraw_component(target_state->component_instance.value());
      }
    } else {
      redraw_component(root);
    }
  }));
  listeners.push_back(win->listen(KeyEvent, {
    auto _pev = this->win->profiling_ctx.scope_event("Key");
    if (focused && focused->component_instance) {
      if (focused->focused) {
        if (ev.data->pressed) {
          INSTANCE_EV_HANDLER(focused)->on_key_press(*ev.data);
        } else if (ev.data->released) {
          INSTANCE_EV_HANDLER(focused)->on_key_release(*ev.data);
        }
      } else {
        focused = nullptr;
      }
    }
  }));
  listeners.push_back(win->listen(ButtonEvent, {
    auto _pev = this->win->profiling_ctx.scope_event("Button");
    
    component_base_t* target = root;
    component_base_t* specified_target = find_by_coords(ev.data->x, ev.data->y);
    if (specified_target) {
      target = specified_target;
    }
    
    auto dim = target->get_dimensional_relations();
    int rel_x = ev.data->x - dim.cx.val();
    int rel_y = ev.data->y - dim.cy.val();
    
    if (focused != target->state.value()) {
      if (focused) {
        //if (focused->component_instance.has_value()) {
        //  focused->component_instance.value()
        //    ->event_handler()
        //    ->on_button_release((Button) it.data->button, 0, 0);
        //}
        focused->focused = false;
        focused = nullptr;
      }
      focused = target->state.value();
      focused->focused = true;
    }
    
    if (ev.data->pressed) {
      target->event_handler()->on_button_press((Button) ev.data->button, rel_x, rel_y);
    } else {
      target->event_handler()->on_button_release((Button) ev.data->button, rel_x, rel_y);
    }
    render_if_dirty(root);
  }));
  listeners.push_back(win->listen(ScrollEvent, {
    auto _pev = this->win->profiling_ctx.scope_event("Scroll");
    component_base_t* target = root;
    component_base_t* specified_target = find_by_coords(ev.data->x, ev.data->y);
    if (specified_target) {
      target = specified_target;
    }
    
    target->event_handler()->on_scroll(ev.data->dx, ev.data->dy);
    
    render_if_dirty(root);
  }));
  listeners.push_back(win->listen(MotionEvent, {
    auto _pev = this->win->profiling_ctx.scope_event("Motion");
    
    if (ev.data->x == -1 && ev.data->y == -1) {
      if (hovering && hovering->component_instance.has_value()) {
        int exit_rel_x = 0;
        int exit_rel_y = 0;
        hovering->hovering = false;
        hovering->component_instance.value()
                ->event_handler()->on_mouse_exit(0, 0);
        hovering = nullptr;
      }
    } else {
      component_base_t* target = root;
      component_base_t* specified_target = find_by_coords(ev.data->x, ev.data->y);
      if (specified_target)
        target = specified_target;
      
      auto dim = target->get_dimensional_relations();
      int rel_x = ev.data->x - dim.cx.val();
      int rel_y = ev.data->y - dim.cy.val();
      
      if (hovering != target->state.value()) {
        if (hovering && hovering->component_instance.has_value()) {
          auto h_dim = hovering->component_instance.value()->get_dimensional_relations();
          int exit_rel_x = ev.data->x - h_dim.cx.val();
          int exit_rel_y = ev.data->y - h_dim.cy.val();
          hovering->hovering = false;
          hovering->component_instance.value()
                  ->event_handler()->on_mouse_exit(exit_rel_x, exit_rel_y);
          hovering = nullptr;
        }
        hovering = target->state.value();
        hovering->hovering = true;
        
        target->event_handler()->on_mouse_enter(rel_x, rel_y);
      } else {
        target->event_handler()->on_mouse_motion(rel_x, rel_y);
      }
    }
    
    // Calling 'Drag' related event handlers
    //cyd::ui::components::Component* target = root;
    //cyd::ui::components::Component* specified_target =
    //  find_by_coords(root, it.data->x, it.data->y);
    //if (specified_target)
    //  target = specified_target;
    //
    //if (it.data->dragging) {
    //  if (dragging_context.dragging) {
    //    int rel_x = it.data->x - (*target->state.unwrap())->dim.cx.val();
    //    int rel_y = it.data->y - (*target->state.unwrap())->dim.cy.val();
    //    dragging_context.dragging_item.drag_move(dragging_context.dragging_item, rel_x, rel_y);
    //    target->on_drag_motion(rel_x, rel_y);
    //  } else {
    //    int rel_x = it.data->x - (*target->state.unwrap())->dim.cx.val();
    //    int rel_y = it.data->y - (*target->state.unwrap())->dim.cy.val();
    //    target->state.let(_(components::ComponentState * , {
    //      for (auto &item : it->draggable_sources) {
    //        if (item.x - 10 <= rel_x && rel_x <= item.x + 10
    //          && item.y - 10 <= rel_y && rel_y <= item.y + 10) {
    //          dragging_context.dragging_item = item.start_drag(rel_x, rel_y);
    //          break;
    //        }
    //      }
    //    }));
    //    target->on_drag_start(rel_x, rel_y);
    //    dragging_context.dragging = true;
    //  }
    //} else if (dragging_context.dragging) {
    //  int rel_x = it.data->x - (*target->state.unwrap())->dim.cx.val();
    //  int rel_y = it.data->y - (*target->state.unwrap())->dim.cy.val();
    //  dragging_context.dragging_item.drag_end(dragging_context.dragging_item, rel_x, rel_y);
    //  target->on_drag_finish(rel_x, rel_y);
    //  dragging_context.dragging = false;
    //  dragging_context.dragging_item = drag_n_drop::draggable_t {};
    //}
    
    render_if_dirty(root);
  }));
  listeners.push_back(win->listen(ResizeEvent, {
    auto _pev = this->win->profiling_ctx.scope_event("Resize");
    log_lay.debug("RESIZE: w=%d, h=%d", ev.data->w, ev.data->h);
    
    auto dim = root->get_dimensional_relations();
    dim.w = ev.data->w;
    dim.h = ev.data->h;
    dim.fixed_w = true;
    dim.fixed_h = true;
    
    redraw_component(root);
  }));
}

