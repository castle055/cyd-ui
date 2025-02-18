/*! \file  layout.cppm
 *! \brief 
 *!
 */

module;
#include <tracy/Tracy.hpp>
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>


export module cydui:layout_ev_dispatch;

import std;
import fabric.logging;
import fabric.profiling;

import cydui.window_events;

export import :layout;

export namespace cyd::ui {
#define INSTANCE_EV_HANDLER(STATE_PTR) \
  if (STATE_PTR->component_instance.has_value()) \
    STATE_PTR->component_instance.value()->get_event_dispatcher()


  std::vector<fabric::async::raw_listener::sptr> Layout::make_event_listeners() {
    std::vector<fabric::async::raw_listener::sptr> listeners { };

    static auto make_listener = [&](auto &&fun) { return win->on_event(fun).raw(); };

    listeners = {
      make_listener([&](const RedrawEvent &ev) {
        ZoneScopedN("Redraw Event");
        auto _pev = this->win->profiling_ctx.scope_event("Redraw");
        if (ev.component) {
          components::component_state_t* target_state = ((components::component_state_t*)ev.component);
          if (target_state->component_instance.has_value()) {
            update_component(target_state->component_instance.value());
          }
        } else {
          update_component(root);
        }

        update_dimensions();
        component_renderer->render(*win->native(), root);
      }),
      make_listener([&](const KeyEvent &ev) {
        ZoneScopedN("Key Event");
        auto _pev = this->win->profiling_ctx.scope_event("Key");
        if (ev.keysym.code == Keycode::SDLK_F12 && ev.pressed && not ev.holding) {
          LOG::print {INFO}("Pressed Debug Key");
          return;
        }
        if (focused && focused->component_instance) {
          if (focused->focused) {
            if (ev.pressed) {
              INSTANCE_EV_HANDLER(focused)->dispatch_key_press(ev);
            } else if (ev.released) {
              INSTANCE_EV_HANDLER(focused)->dispatch_key_release(ev);
            }
            render_if_dirty(focused->component_instance.value());
          } else {
            focused = nullptr;
          }
        }
      }),
      make_listener([&](const TextInputEvent &ev) {
        ZoneScopedN("Key Event");
        auto _pev = this->win->profiling_ctx.scope_event("Key");
        if (focused && focused->component_instance) {
          if (focused->focused) {
            INSTANCE_EV_HANDLER(focused)->dispatch_text_input(ev);
            render_if_dirty(focused->component_instance.value());
          } else {
            focused = nullptr;
          }
        }
      }),
      make_listener([&](const ButtonEvent &ev) {
        ZoneScopedN("Button Event");
        auto _pev = this->win->profiling_ctx.scope_event("Button");

        components::component_base_t* target           = root.get();
        components::component_base_t* specified_target = find_by_coords(ev.x, ev.y);
        if (specified_target) {
          target = specified_target;
        }

        auto &dim     = target->get_dimensional_relations();
        auto &int_rel = target->get_internal_relations();
        auto rel_x    = ev.x - dimensions::get_value(int_rel.cx);
        auto rel_y    = ev.y - dimensions::get_value(int_rel.cy);

        if (focused != target->state()) {
          if (focused) {
            // if (focused->component_instance.has_value()) {
            //   focused->component_instance.value()
            //     ->event_handler()
            //     ->on_button_release((Button) it.button, 0, 0);
            // }
            focused->focused = false;
            focused->mark_dirty();
            focused = nullptr;
          }
          focused          = target->state();
          focused->focused = true;
          focused->mark_dirty();
        }

        if (ev.pressed) {
          target->get_event_dispatcher()->dispatch_button_press((Button)ev.button, rel_x, rel_y);
        } else {
          target->get_event_dispatcher()->dispatch_button_release((Button)ev.button, rel_x, rel_y);
        }
        render_if_dirty(root);
      }),
      make_listener([&](const ScrollEvent &ev) {
        ZoneScopedN("Scroll Event");
        auto _pev                                      = this->win->profiling_ctx.scope_event("Scroll");
        components::component_base_t* target           = root.get();
        components::component_base_t* specified_target = find_by_coords(ev.x, ev.y);
        if (specified_target) {
          target = specified_target;
        }

        target->get_event_dispatcher()->dispatch_scroll(ev.dx, ev.dy);

        render_if_dirty(root);
      }),
      make_listener([&](const MotionEvent &ev) {
        auto _pev = this->win->profiling_ctx.scope_event("Motion");
        ZoneScopedN("MotionEvent");

        if (ev.x == dimensions::screen_measure {-1} && ev.y == dimensions::screen_measure {-1}) {
          clear_hovering_flag(root_state, ev);
        } else {
          components::component_base_t* target           = root.get();
          components::component_base_t* specified_target = find_by_coords(ev.x, ev.y);
          if (specified_target)
            target = specified_target;

          if (not set_hovering_flag(target->state().get(), ev, true)) {
            auto &int_rel = target->get_internal_relations();
            auto rel_x    = ev.x - dimensions::get_value(int_rel.cx);
            auto rel_y    = ev.y - dimensions::get_value(int_rel.cy);
            target->get_event_dispatcher()->dispatch_mouse_motion(rel_x, rel_y);
          }
        }

        // Calling 'Drag' related event handlers
        // cyd::ui::components::Component* target = root;
        // cyd::ui::components::Component* specified_target =
        //  find_by_coords(root, it.x, it.y);
        // if (specified_target)
        //  target = specified_target;
        //
        // if (it.dragging) {
        //  if (dragging_context.dragging) {
        //    int rel_x = it.x - (*target->state.unwrap())->dim.cx.val();
        //    int rel_y = it.y - (*target->state.unwrap())->dim.cy.val();
        //    dragging_context.dragging_item.drag_move(dragging_context.dragging_item, rel_x,
        //    rel_y); target->on_drag_motion(rel_x, rel_y);
        //  } else {
        //    int rel_x = it.x - (*target->state.unwrap())->dim.cx.val();
        //    int rel_y = it.y - (*target->state.unwrap())->dim.cy.val();
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
        //  int rel_x = it.x - (*target->state.unwrap())->dim.cx.val();
        //  int rel_y = it.y - (*target->state.unwrap())->dim.cy.val();
        //  dragging_context.dragging_item.drag_end(dragging_context.dragging_item, rel_x, rel_y);
        //  target->on_drag_finish(rel_x, rel_y);
        //  dragging_context.dragging = false;
        //  dragging_context.dragging_item = drag_n_drop::draggable_t {};
        //}

        render_if_dirty(root);
      }),
      make_listener([&](const ResizeEvent &ev) {
        ZoneScopedN("Resize Event");
        auto _pev = this->win->profiling_ctx.scope_event("Resize");

        auto &dim   = root->get_dimensional_relations();
        dim._width  = ev.w;
        dim._height = ev.h;

        update_dimensions();
        // if (is_compositing.test()) {
        //   composite_is_outdated.test_and_set();
        //   return;
        // }
        // update_fragments();
        // render();
      }),
    };

    return listeners;
  }
}
