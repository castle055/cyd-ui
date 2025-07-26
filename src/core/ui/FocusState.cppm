/*! \file  FocusState.cppm
 *! \brief
 *!
 */

module;
#include "../../debug/profiling/macros.h"
#define PROF_CURRENT_MODULE cydui::core::focus_state

#include <cyd_fabric_modules/headers/macros/async_events.h>

export module cydui.core.focus_state;
export import cydui.core.frame_controller;
export import cydui.core.style;

import std;
export import reflect;
import cydui.debug.profiling;

namespace cydui::core {
  export EVENT(RequestComponentFocus) {
    mounted_component_t* component;
  };

  export class FocusState {
    FrameController&                                       frame_;
    StyleStore&                                       style_;
    std::optional<mounted_component_t*> focused_{std::nullopt};

  public:
    explicit FocusState(
      FrameController& frame,
      StyleStore& style
    )
        : frame_(frame),
          style_(style) {}

    std::optional<mounted_component_t*> get_focused() {
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
        frame_.stop_text_input();
      }
      focused.mark_dirty();

      focused_ = std::nullopt;
    }

    void focus(mounted_component_t& component) {
      PROF_SCOPE(focus);
      if (focused_ == &component) {
        return;
      }
      unfocus();

      focused_      = &component;
      auto& focused = *focused_.value();

      focused.focus();
      if (focused.is_text_input()) {
        frame_.start_text_input();
      }
      focused.mark_dirty();
    }
  };
} // namespace cydui
