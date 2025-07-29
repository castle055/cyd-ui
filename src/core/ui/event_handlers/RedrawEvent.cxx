/*! \file  RedrawEvent.cxx
 *! \brief
 *!
 */

module;
#include <tracy/Tracy.hpp>

module cydui.core.ui.services.UIEventHandler;
using namespace cydui::detail::ui::services;

fabric::task<> UIEventHandler::onRedrawEvent(const RedrawEvent& ev) {
  ZoneScopedN("Redraw Event");
  updater_.schedule_update();
  co_return;
}
