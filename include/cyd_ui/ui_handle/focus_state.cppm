/*! \file  focus_state.cppm
 *! \brief
 *!
 */

module;
#include "cyd_ui/debug/profiling/macros.h"
#define PROF_CURRENT_MODULE cydui::layout::focus_state

#include <cyd_fabric_modules/headers/macros/async_events.h>

export module cydui.layout.focus_state;
export import cydui.layout.ui_frame;
export import cydui.layout.ui_style;

import std;
export import reflect;
import cydui.debug.profiling;

namespace cydui {
  export EVENT(RequestComponentFocus) {
    components::mounted_component_t* component;
  };

  export class focus_state {
    ui_frame&                                       frame_;
    ui_style&                                       style_;
    std::optional<components::mounted_component_t*> focused_{std::nullopt};

  public:
    explicit focus_state(
      ui_frame& frame,
      ui_style& style
    )
        : frame_(frame),
          style_(style) {}

    std::optional<components::mounted_component_t*> get_focused() {
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

    void focus(components::mounted_component_t& component) {
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
