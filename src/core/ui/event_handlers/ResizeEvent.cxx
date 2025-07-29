/*! \file  ResizeEvent.cxx
 *! \brief
 *!
 */

module;
#include <tracy/Tracy.hpp>

module cydui.core.ui.services.UIEventHandler;
using namespace cydui::detail::ui::services;

fabric::task<> UIEventHandler::onResizeEvent(const ResizeEvent& ev) {
  ZoneScopedN("Resize Event");
  frame_.update_size(ev.w.value_as_base_unit(), ev.h.value_as_base_unit());
  tree_.update_root_size(ev.w, ev.h);
  co_return;

  // updater_.reflow();
  // updater_.schedule_update();
}
