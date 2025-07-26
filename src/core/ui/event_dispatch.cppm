/*! \file  event_dispatch.cppm
 *! \brief
 *!
 */

module;
#include <tracy/Tracy.hpp>
#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>

export module cydui.core.ev_dispatch;
export import cydui.core.tree;
export import cydui.core.style;
export import cydui.core.hover_state;
export import cydui.core.focus_state;
export import cydui.core.updater;

import std;
import fabric.logging;
import fabric.profiling;


namespace cydui::core {
  export class event_dispatcher {
    FrameController&    frame_;
    ComponentTree&     tree_;
    StyleStore&    style_;
    HoverState& hover_state_;
    FocusState& focus_state_;
    UIUpdater&  updater_;

    std::vector<fabric::async::raw_listener::sptr> listeners{};

  public:
    event_dispatcher(
      FrameController&    frame,
      ComponentTree&     tree,
      StyleStore&    style,
      HoverState& hover_state,
      FocusState& focus_state,
      UIUpdater&  updater
    )
        : frame_(frame),
          tree_(tree),
          style_(style),
          hover_state_(hover_state),
          focus_state_(focus_state),
          updater_(updater) {}

    ~event_dispatcher() {
      remove_all_listeners();
    }

    void start_listeners() {
      make_event_listeners();
    }

    void stop_listeners() {
      remove_all_listeners();
    }

  private:
    void remove_all_listeners() {
      for (auto& item: listeners) {
        item->remove();
      }
      listeners.clear();
    }

    void make_event_listeners();
  };
} // namespace cydui

export namespace cydui::core {
  void event_dispatcher::make_event_listeners() {
    ZoneScopedN("Layout:make_event_listeners");
    remove_all_listeners();

    static auto make_listener = [&](auto&& fun) { return frame_.get_frame()->on_event(fun).raw(); };

    listeners = {
      make_listener([&](const RequestComponentFocus& ev) -> fabric::task<> {
        const auto& target = ev.component;
        if (nullptr == target) {
          focus_state_.unfocus();
        } else {
          focus_state_.focus(*target);
        }
        updater_.schedule_update();
        co_return;
      }),
      make_listener([&](const WindowClosed& ev) -> fabric::task<> {
        // frame_.get_frame()->terminate();
        co_return;
      }),
      make_listener([&](const RedrawEvent& ev) -> fabric::task<> {
        ZoneScopedN("Redraw Event");
        updater_.schedule_update();
        co_return;
      }),
      make_listener([&](const KeyEvent& ev) -> fabric::task<> {
        ZoneScopedN("Key Event");
        if (ev.keysym.code == SDLK_F12 && ev.pressed && not ev.holding) {
          LOG::print{INFO}("Pressed Debug Key");
          co_return;
        }
        auto focused_opt = focus_state_.get_focused();
        if (focused_opt.has_value()) {
          auto& focused_evd = focused_opt.value()->get_event_dispatcher();
          if (ev.pressed) {
            focused_evd.dispatch_key_press(ev);
          } else if (ev.released) {
            focused_evd.dispatch_key_release(ev);
          }
          updater_.schedule_update();
        }
        co_return;
      }),
      make_listener([&](const TextInputEvent& ev) -> fabric::task<> {
        ZoneScopedN("Text Input Event");
        auto focused_opt = focus_state_.get_focused();
        if (focused_opt.has_value()) {
          focused_opt.value()->get_event_dispatcher().dispatch_text_input(ev);
          updater_.schedule_update();
        }
        co_return;
      }),
      make_listener([&](const ButtonEvent& ev) -> fabric::task<> {
        ZoneScopedN("Button Event");

        hover_state_.update_mouse_position(ev.x, ev.y);

        mounted_component_t* target           = tree_.root.get();
        mounted_component_t* specified_target = tree_.find_by_coords(ev.x, ev.y);
        if (specified_target) {
          target = specified_target;
        }
        focus_state_.focus(*target);

        auto& geom          = target->get_geometry();
        auto [rel_x, rel_y] = geom.get_relative(ev.x, ev.y);

        if (ev.pressed) {
          target->get_event_dispatcher().dispatch_button_press((Button)ev.button, rel_x, rel_y);
        } else {
          target->get_event_dispatcher().dispatch_button_release((Button)ev.button, rel_x, rel_y);
        }
        updater_.schedule_update();
        co_return;
      }),
      make_listener([&](const ScrollEvent& ev) -> fabric::task<> {
        ZoneScopedN("Scroll Event");

        mounted_component_t* target           = tree_.root.get();
        mounted_component_t* specified_target = tree_.find_by_coords(ev.x, ev.y);
        if (specified_target) {
          target = specified_target;
        }

        target->get_event_dispatcher().dispatch_scroll(ev.dx, ev.dy);
        hover_state_.update_mouse_position(ev.x, ev.y);
        updater_.schedule_update();
        co_return;
      }),
      make_listener([&](const MotionEvent& ev) -> fabric::task<> {
        ZoneScopedN("MotionEvent");

        hover_state_.update_mouse_position(ev.x, ev.y);

        if (ev.x != dimensions::screen_measure{-1} and ev.y != dimensions::screen_measure{-1}) {
          mounted_component_t* target           = tree_.root.get();
          mounted_component_t* specified_target = tree_.find_by_coords(ev.x, ev.y);
          if (specified_target)
            target = specified_target;

          auto& geom          = target->get_geometry();
          auto [rel_x, rel_y] = geom.get_relative(ev.x, ev.y);
          target->get_event_dispatcher().dispatch_mouse_motion(rel_x, rel_y);
        }

        // Calling 'Drag' related event handlers
        // cydui::components::Component* target = root;
        // cydui::components::Component* specified_target =
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

        updater_.schedule_update();
        co_return;
      }),
      make_listener([&](const ResizeEvent& ev) -> fabric::task<> {
        ZoneScopedN("Resize Event");
        tree_.set_root_size(ev.w, ev.h);

        // updater_.reflow();
        // updater_.schedule_update();
        co_return;
      }),
    };
  }
} // namespace cydui
