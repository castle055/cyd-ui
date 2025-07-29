/*! \file  MotionEvent.cxx
 *! \brief
 *!
 */

module;
#include <tracy/Tracy.hpp>

module cydui.core.ui.services.UIEventHandler;
using namespace cydui::detail::ui::services;

fabric::task<> UIEventHandler::onMotionEvent(const MotionEvent& ev) {
  ZoneScopedN("MotionEvent");
  hover_state_.update_mouse_position(ev.x, ev.y);

  if (ev.x != dimensions::screen_measure {-1} and ev.y != dimensions::screen_measure {-1}) {
    ComponentImpl& target = tree_.find_by_coords(ev.x, ev.y);

    auto grabbed = focus_state_.get_grabbed();
    if (grabbed.has_value()) {
      auto& grabbed_c     = *grabbed.value();
      auto [rel_x, rel_y] = grabbed_c.get_geometry().get_relative(ev.x, ev.y);
      grabbed_c.get_event_dispatcher().dispatch_mouse_motion(rel_x, rel_y);
    } else {
      auto [rel_x, rel_y] = target.get_geometry().get_relative(ev.x, ev.y);
      target.get_event_dispatcher().dispatch_mouse_motion(rel_x, rel_y);
    }
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
}
