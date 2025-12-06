/*! \file  PlatformImpl.cppm
 *! \brief
 *!
 */

export module cydui.platform.impl;

import std;
import reflect;

export import fabric.async;

export import cydui.platform;
export import cydui.platform.window.window_options_base;
export import cydui.platform.render.renderer_options_base;

import cydui.platform.window.window_base;
import cydui.platform.render.renderer_base;

namespace cydui::platform {
  export class PlatformImpl final: public Platform {
    std::shared_ptr<fabric::async::async_bus_t> bus_;
    fabric::services::ServiceContext::sptr      service_context_;

  public:
    using sptr = std::shared_ptr<PlatformImpl>;

    PlatformImpl(
      const fabric::async::async_bus_t::sptr&       bus,
      const fabric::services::ServiceContext::sptr& service_context);

    fabric::task<window::WindowBase&>   get_window();
    fabric::task<render::RendererBase&> get_renderer();

    fabric::tasks::executor::sptr get_executor() override;

    fabric::services::ServiceContext::sptr get_service_context();

    fabric::async::async_bus_t& get_bus() override;

    fabric::task<sptr> make_child_platform(
      window::WindowType                       window_type,
      const window::WindowOptionsBase::sptr&   window_options,
      const render::RendererOptionsBase::sptr& renderer_options);
  };
} // namespace cydui::platform
