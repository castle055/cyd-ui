/*! \file  Platform.cxx
 *! \brief
 *!
 */

module cydui.platform;

import fabric.main;
import cydui.platform.impl;
import cydui.platform.render.renderer_base;
import cydui.platform.render.renderer_factory;
import cydui.platform.window.window_base;
import cydui.platform.window.window_factory;

using namespace cydui;
using namespace cydui::platform;
using namespace window;
using namespace render;

fabric::task<Platform::sptr> make_impl(
  const WindowOptionsBase::sptr&   window_opts,
  const RendererOptionsBase::sptr& renderer_opts
) {
  auto bus = std::make_shared<fabric::async::async_bus_t>();
  co_await fabric::this_task::switch_executor(bus->get_executor());

  auto ctx = co_await fabric::services::ServiceContext::make<services::WindowScope>(
    fabric::runtime::get_global_service_context(), {"WindowContext"}
  );

  co_await register_window_service(ctx, window_opts);
  co_await register_render_service(ctx, renderer_opts);
  co_await fabric::runtime::get_global_service_context()->await_ready();
  co_await ctx->await_ready();

  auto& window = co_await ctx->require<WindowBase>();

  co_await fabric::launch(window.event_task(bus));

  co_return std::make_shared<PlatformImpl>(bus, ctx);
}
