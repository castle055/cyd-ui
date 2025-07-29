/*! \file  Platform.cppm
 *! \brief
 *!
 */

export module cydui.platform;

import std;
import reflect;

export import fabric.async;
export import fabric.services;

export import cydui.platform.window.window_options_base;
export import cydui.platform.render.renderer_options_base;

namespace cydui {
  export class Platform;
}

fabric::task<std::shared_ptr<cydui::Platform>> make_impl(
  const cydui::platform::window::WindowOptionsBase::sptr&   window_opts,
  const cydui::platform::render::RendererOptionsBase::sptr& renderer_opts
);

namespace cydui {
  class Platform {
  public:
    using sptr = std::shared_ptr<Platform>;

    static fabric::task<std::shared_ptr<Platform>> make(
      const platform::window::WindowOptionsConcept auto&   window_opts,
      const platform::render::RendererOptionsConcept auto& renderer_opts
    ) {
      platform::window::WindowOptionsBase::sptr w_opts =
        std::make_shared<std::remove_cvref_t<decltype(window_opts)>>(window_opts);
      platform::render::RendererOptionsBase::sptr r_opts =
        std::make_shared<std::remove_cvref_t<decltype(renderer_opts)>>(renderer_opts);
      co_return co_await make_impl(w_opts, r_opts);
    }

    virtual ~Platform() = default;

    virtual fabric::tasks::executor::sptr get_executor() = 0;

    virtual fabric::async::async_bus_t& get_bus() = 0;
  };
} // namespace cydui
