/*! \file  PlatformImpl.cxx
 *! \brief
 *!
 */

module cydui.platform.impl;

import std;
import reflect;

import fabric.async;
import fabric.services;
import fabric.main;

import cydui.platform.window.window_base;
import cydui.platform.render.renderer_base;

import cydui.platform.window.window_factory;
import cydui.platform.render.renderer_factory;
import cydui.service_scopes;

using namespace cydui::platform;
using namespace cydui::platform::render;
using namespace cydui::platform::window;

PlatformImpl::PlatformImpl(
  const std::shared_ptr<fabric::async::async_bus_t>& bus,
  const fabric::services::ServiceContext::sptr&      service_context)
    : bus_(bus),
      service_context_(service_context) {}

fabric::task<WindowBase&> PlatformImpl::get_window() {
  co_return co_await service_context_->require<WindowBase>();
}

fabric::task<RendererBase&> PlatformImpl::get_renderer() {
  co_return co_await service_context_->require<RendererBase>();
}

fabric::async::async_bus_t& PlatformImpl::get_bus() {
  return *bus_;
}

fabric::tasks::executor::sptr PlatformImpl::get_executor() {
  return bus_->get_executor();
}

fabric::services::ServiceContext::sptr PlatformImpl::get_service_context() {
  return service_context_;
}

fabric::task<PlatformImpl::sptr> PlatformImpl::make_child_platform(
  WindowType                       window_type,
  const WindowOptionsBase::sptr&   window_options,
  const RendererOptionsBase::sptr& renderer_options) {
  auto bus = std::make_shared<fabric::async::async_bus_t>();
  co_await fabric::this_task::switch_executor(bus->get_executor());

  auto ctx = co_await fabric::services::ServiceContext::make<services::WindowScope>(
    fabric::runtime::get_global_service_context(), {"WindowContext"});

  auto window = service_context_->find<WindowBase>().value();

  co_await register_window_service(ctx, window_type, window, window_options);
  co_await register_render_service(ctx, renderer_options);
  co_await fabric::runtime::get_global_service_context()->await_ready();
  co_await ctx->await_ready();

  auto& child_window = co_await ctx->require<WindowBase>();

  fabric::launch(child_window.event_task(bus)).detach();

  co_return std::make_shared<PlatformImpl>(std::move(bus), std::move(ctx));
}
