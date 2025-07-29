/*! \file  FocusState.cppm
 *! \brief
 *!
 */

module;
#include "../../../debug/profiling/macros.h"
#define PROF_CURRENT_MODULE cydui::core::ui::services::FocusState

export module cydui.core.ui.services.FocusState;

import std;
export import reflect;
export import fabric.services;
import cydui.debug.profiling;

export import cydui.core.ui.services.PlatformController;
export import cydui.core.ui.services.StyleStore;

namespace cydui::detail::ui::services {
  export class FocusState final: public fabric::services::ServiceBase {
    PlatformController&           platform_;
    StyleStore&                   style_;
    std::optional<ComponentImpl*> focused_ {std::nullopt};
    std::optional<ComponentImpl*> mouse_grabbed_ {std::nullopt};

    FocusState(
      PlatformController& platform,
      StyleStore&         style)
        : platform_(platform),
          style_(style) {}

  public:
    static fabric::task<sptr> start(fabric::services::ServiceLocator& locator) {
      auto& platform = co_await locator.require<PlatformController>();
      auto& style    = co_await locator.require<StyleStore>();

      co_return sptr {new FocusState(platform, style)};
    }

    std::optional<ComponentImpl*> get_focused() {
      PROF_SCOPE(get_focused);
      return focused_;
    }

    void unfocus() {
      PROF_SCOPE(unfocus);
      if (focused_ == std::nullopt) {
        return;
      }
      auto& focused = *focused_.value();

      focused.unfocus();
      if (focused.is_text_input()) {
        fabric::launch(platform_.stop_text_input()).detach();
      }
      focused.mark_dirty();

      focused_ = std::nullopt;
    }

    fabric::task<> focus(ComponentImpl& component) {
      PROF_SCOPE(focus);
      if (focused_ == &component) {
        co_return;
      }
      unfocus();

      focused_      = &component;
      auto& focused = *focused_.value();

      focused.focus();
      if (focused.is_text_input()) {
        co_await platform_.start_text_input();
      }
      focused.mark_dirty();
    }

    void ungrab() {
      mouse_grabbed_ = std::nullopt;
    }

    void grab(ComponentImpl& component) {
      mouse_grabbed_ = &component;
    }

    std::optional<ComponentImpl*> get_grabbed() {
      return mouse_grabbed_;
    }
  };
} // namespace cydui::detail::ui::services
