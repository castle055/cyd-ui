/*! \file  ScrollEvent.cxx
 *! \brief
 *!
 */

module;
#include <tracy/Tracy.hpp>

module cydui.core.ui.services.UIEventHandler;
using namespace cydui::detail::ui::services;

fabric::task<> UIEventHandler::onScrollEvent(const ScrollEvent& ev) {
  ZoneScopedN("Scroll Event");
  ComponentImpl& target = tree_.find_by_coords(ev.x, ev.y);
  target.get_event_dispatcher().dispatch_scroll(ev.dx, ev.dy);
  hover_state_.update_mouse_position(ev.x, ev.y);
  updater_.schedule_update();
  co_return;
}
