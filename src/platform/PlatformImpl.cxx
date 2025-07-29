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
  const fabric::services::ServiceContext::sptr&      service_context
)
    : bus_(bus),
      service_context_(service_context) {
}

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