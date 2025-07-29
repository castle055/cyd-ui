/*! \file  KeyEvent.cxx
 *! \brief
 *!
 */

module;
#include <tracy/Tracy.hpp>

module cydui.core.ui.services.UIEventHandler;
using namespace cydui::detail::ui::services;

fabric::task<> UIEventHandler::onKeyEvent(const KeyEvent& ev) {
  ZoneScopedN("Key Event");
  if (ev.code == Keycode::F12 && ev.pressed && not ev.holding) {
    LOG::print {INFO}("Pressed Debug Key");
    frame_.get_bus().emit(DebugKeyPressed {});
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
}
