/*! \file  ButtonEvent.cxx
 *! \brief
 *!
 */

module;
#include <tracy/Tracy.hpp>

module cydui.core.ui.services.UIEventHandler;
using namespace cydui::detail::ui::services;

fabric::task<> UIEventHandler::onButtonEvent(const ButtonEvent& ev) {
  ZoneScopedN("Button Event");
  hover_state_.update_mouse_position(ev.x, ev.y);

  ComponentImpl& target = tree_.find_by_coords(ev.x, ev.y);

  auto& geom          = target.get_geometry();
  auto [rel_x, rel_y] = geom.get_relative(ev.x, ev.y);

  if (ev.pressed) {
    co_await focus_state_.focus(target);
    focus_state_.grab(target);
    target.get_event_dispatcher().dispatch_button_press((Button)ev.button, rel_x, rel_y);
  } else {
    auto grabbed = focus_state_.get_grabbed();
    if (grabbed.has_value()) {
      auto& grabbed_c     = *grabbed.value();
      auto [rel_x, rel_y] = grabbed_c.get_geometry().get_relative(ev.x, ev.y);
      grabbed_c.get_event_dispatcher().dispatch_button_release((Button)ev.button, rel_x, rel_y);
      focus_state_.ungrab();
    } else {
      target.get_event_dispatcher().dispatch_button_release((Button)ev.button, rel_x, rel_y);
    }
  }
  updater_.schedule_update();
  co_return;
}
