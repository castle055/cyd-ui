/*! \file  PlatformController.cppm
 *! \brief
 *!
 */

module;
#include "../../../debug/profiling/macros.h"
#define PROF_CURRENT_MODULE cydui::core::ui::services::PlatformController

export module cydui.core.ui.services.PlatformController;

import std;
export import reflect;
export import fabric.services;
import cydui.debug.profiling;

import cydui.platform.impl;
import cydui.platform.render.renderer_base;
import cydui.platform.window.window_base;

namespace cydui::detail::ui::services {
  export class PlatformController final: public fabric::services::ServiceBase {
    platform::PlatformImpl&         platform_;
    platform::render::RendererBase& renderer_;
    platform::window::WindowBase&   window_;
    std::pair<int, int>             size_ {};

    PlatformController(
      platform::PlatformImpl&         platform,
      platform::render::RendererBase& renderer,
      platform::window::WindowBase&   window,
      std::pair<
        int,
        int> size)
        : platform_(platform),
          renderer_(renderer),
          window_(window),
          size_(size) {}

  public:
    static fabric::task<sptr> start(
      fabric::services::ServiceLocator& locator,
      platform::PlatformImpl*           platform) {
      auto& renderer = co_await locator.require<platform::render::RendererBase>();
      auto& window   = co_await locator.require<platform::window::WindowBase>();

      co_return sptr {new PlatformController(*platform, renderer, window, co_await window.get_size())};
    }

    fabric::task<> start_text_input() {
      co_await window_.enable_text_input();
      PROF_MESSAGE("Text Input Started");
    }

    fabric::task<> stop_text_input() {
      co_await window_.disable_text_input();
      PROF_MESSAGE("Text Input Stopped");
    }

    fabric::async::async_bus_t& get_bus() {
      return platform_.get_bus();
    }

    std::pair<
      int,
      int>
    get_size() {
      return size_;
    }

    void update_size(
      int w,
      int h) {
      size_ = {w, h};
    }

    platform::window::WindowBase& get_window() {
      return window_;
    }

    platform::render::RendererBase& get_renderer() {
      return renderer_;
    }

    fabric::task<platform::render::FrameRendererBase::uptr> begin_frame() {
      PROF_SCOPE(Begin Frame);
      auto [width, height] = size_;
      co_return co_await renderer_.begin_frame(width, height);
    }

    fabric::task<> present(const platform::Surface& frame_buffer) {
      co_await window_.present(frame_buffer);
    }
  };
} // namespace cydui::detail::ui::services
