/*! \file  Platform.cxx
 *! \brief
 *!
 */

module cydui.platform;

import std;
import reflect;

import fabric.async;

import cydui.platform.window.window_base;
import cydui.platform.render.renderer_base;

import cydui.platform.window.window_factory;
import cydui.platform.render.renderer_factory;

using namespace cydui::platform::render;
using namespace cydui::platform::window;

namespace cydui::platform {
  class PlatformImpl: public fabric::async::async_bus_t {
    WindowBase::sptr   window;
    RendererBase::sptr renderer;

  public:
    explicit PlatformImpl(
      const WindowOptionsBase::sptr&   window_opts,
      const RendererOptionsBase::sptr& renderer_opts
    )
        : window(make_window(window_opts)),
          renderer(make_renderer(renderer_opts)) {
      start_event_task();
    }

    const WindowBase::sptr& get_window() {
      return window;
    }

  private:
    void start_event_task() {
      get_executor()->schedule([this] -> fabric::task<> {
        co_await this->window->event_task();
        co_return;
      });
    }
  };
} // namespace cydui::platform

namespace cydui::platform {
  Platform::Platform(
    const WindowOptionsBase::sptr&   window_opts,
    const RendererOptionsBase::sptr& renderer_opts
  )
      : impl_(
          std::make_unique<PlatformImpl>(
            window_opts,
            renderer_opts
          )
        ) {}
} // namespace cydui::platform
