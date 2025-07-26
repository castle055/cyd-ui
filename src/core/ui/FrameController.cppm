/*! \file  frame_controller.cppm
 *! \brief
 *!
 */

module;
#include "../../debug/profiling/macros.h"
#define PROF_CURRENT_MODULE cydui::core::frame_controller

export module cydui.core.frame_controller;

import std;
export import reflect;
import cydui.debug.profiling;

import cydui.backends;

namespace cydui::core {
  export class FrameController {
    backends::frame_base::sptr frame_;
  public:
    explicit FrameController(const backends::frame_base::sptr& frame) : frame_(frame) {}

    void start_text_input() {
      frame_->enable_text_input();
      PROF_MESSAGE("Text Input Started");
    }
    void stop_text_input() {
      frame_->disable_text_input();
      PROF_MESSAGE("Text Input Stopped");
    }

    auto& get_bus() {
      return *std::dynamic_pointer_cast<fabric::async::async_bus_t>(frame_);
    }

    auto get_frame() {
      return frame_;
    }

    std::pair<int, int> get_size() {
      return frame_->get_size();
    }

    auto get_renderer() {
      return frame_->get_renderer();
    }
  };
} // namespace cydui
