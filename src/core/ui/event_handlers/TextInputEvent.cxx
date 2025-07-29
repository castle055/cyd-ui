/*! \file  TextInputEvent.cxx
 *! \brief
 *!
 */

module;
#include <tracy/Tracy.hpp>

module cydui.core.ui.services.UIEventHandler;
using namespace cydui::detail::ui::services;

fabric::task<> UIEventHandler::onTextInputEvent(const TextInputEvent& ev) {
  ZoneScopedN("Text Input Event");
  auto focused_opt = focus_state_.get_focused();
  if (focused_opt.has_value()) {
    focused_opt.value()->get_event_dispatcher().dispatch_text_input(ev);
    updater_.schedule_update();
  }
  co_return;
}
