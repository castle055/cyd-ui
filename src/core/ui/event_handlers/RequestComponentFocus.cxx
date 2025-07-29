/*! \file  RequestComponentFocus.cxx
 *! \brief 
 *!
 */

module;
#include <tracy/Tracy.hpp>

module cydui.core.ui.services.UIEventHandler;
using namespace cydui::detail::ui::services;

fabric::task<> UIEventHandler::onRequestComponentFocus(const RequestComponentFocus& ev) {
  const auto& target = ev.component;
  if (nullptr == target) {
    focus_state_.unfocus();
  } else {
    co_await focus_state_.focus(*ComponentImpl::from_interface(target));
  }
  updater_.schedule_update();
}
