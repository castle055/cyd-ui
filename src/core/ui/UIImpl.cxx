/*! \file  UIImpl.cxx
 *! \brief
 *!
 */

module;
#include <tracy/Tracy.hpp>

module cydui.ui_handle.impl;

import std;
import fabric.logging;
import fabric.main;
import fabric.profiling;

import cydui.dimensions;
import cydui.styling.lang;
import cydui.core.blueprint;
import cydui.geometric_relations;

import cydui.core.ui.services.PlatformController;
import cydui.core.ui.services.StyleStore;
import cydui.core.ui.services.FocusState;
import cydui.core.ui.services.ComponentTree;
import cydui.core.ui.services.HoverState;
import cydui.core.ui.services.UIRenderer;
import cydui.core.ui.services.UIUpdater;
import cydui.core.ui.services.UIEventHandler;
import cydui.core.ui.services.AnimationService;
import cydui.service_scopes;

using namespace cydui;
using namespace cydui::detail;
using namespace cydui::detail::ui::services;

fabric::task<UIImpl::sptr> UIImpl::make(
  platform::PlatformImpl::sptr platform,
  Blueprint::uptr              root_blueprint,
  const UIOptions&             options) {
  co_await fabric::this_task::switch_executor(platform->get_executor());

  auto service_context = co_await fabric::services::ServiceContext::make<services::UIScope>(
    platform->get_service_context(), {"InternalUIContext"});

  auto ui_service_context = co_await fabric::services::ServiceContext::make<services::UIScope>(
    fabric::runtime::get_global_service_context(), {"UIContext"});

  co_await service_context->register_service<PlatformController>(platform.get());
  co_await service_context->await_ready();
  co_await service_context->register_service<StyleStore>();
  co_await service_context->await_ready();
  co_await service_context->register_service<FocusState>();
  co_await service_context->await_ready();

  ComponentImpl::uptr root = ComponentImpl::make_root(
    platform->get_bus(), *service_context, *ui_service_context, std::move(root_blueprint));
  root->mark_dirty();

  co_await service_context->register_service<ComponentTree>(&root); // gets moved out
  co_await service_context->await_ready();
  co_await service_context->register_service<HoverState>();
  co_await service_context->await_ready();
  co_await service_context->register_service<UIRenderer>();
  co_await service_context->await_ready();
  co_await service_context->register_service<UIUpdater>();
  co_await service_context->await_ready();
  co_await service_context->register_service<UIEventHandler>();
  co_await service_context->await_ready();
  co_await service_context->register_service<AnimationService>();
  co_await service_context->await_ready();

  auto& style   = co_await service_context->require<StyleStore>();
  auto& tree    = co_await service_context->require<ComponentTree>();
  auto& updater = co_await service_context->require<UIUpdater>();

  for (const auto& stylesheet: options.stylesheets) {
    co_await style.attach_stylesheet(stylesheet);
  }
  tree.update_root_size();
  tree.update_style();
  updater.schedule_update();


  co_return std::make_shared<UIImpl>(std::move(platform), service_context, ui_service_context);
}

UIImpl::UIImpl(
  platform::PlatformImpl::sptr                  platform,
  const fabric::services::ServiceContext::sptr& service_context,
  const fabric::services::ServiceContext::sptr& ui_service_context)
    : platform_(std::move(platform)),
      service_context_(service_context),
      ui_service_context_(ui_service_context),
      window_closed_listener_(platform_->get_bus().on_event([this](WindowCloseRequested) -> fabric::task<> {
        co_await ui_service_context_->stop_all();
        co_await service_context_->stop_all();
        co_await platform_->get_service_context()->stop_all();
        platform_->get_bus().emit<WindowClosed>();
        // co_await fabric::this_task::switch_executor(fabric::runtime::get_main_executor());
        // platform_->get_bus()->request_stop();
        // platform_->get_bus()->join();
      })) {}

UIImpl::~UIImpl() {
  auto exec = platform_->get_executor();
  exec->schedule(ui_service_context_->stop_all());
  exec->schedule(service_context_->stop_all());
}


fabric::task<> UIImpl::show() {
  co_await fabric::this_task::switch_executor(platform_->get_executor());

  if (showing_) {
    co_return;
  }
  showing_ = true;

  auto& updater       = co_await service_context_->require<UIUpdater>();
  auto& event_handler = co_await service_context_->require<UIEventHandler>();

  updater.schedule_update();
  co_await event_handler.start_listeners();
}

fabric::task<> UIImpl::until_closed() {
  co_await platform_->get_bus().await<WindowClosed>();
}

fabric::async::async_bus_t& UIImpl::bus() {
  return platform_->get_bus();
}


fabric::task<> UIImpl::attach_stylesheet(const tss::StyleSheet::sptr& style_sheet) {
  co_await fabric::this_task::switch_executor(platform_->get_executor());

  auto& style    = co_await service_context_->require<StyleStore>();
  auto& tree     = co_await service_context_->require<ComponentTree>();
  auto& updater  = co_await service_context_->require<UIUpdater>();
  auto& renderer = co_await service_context_->require<UIRenderer>();

  co_await style.attach_stylesheet(style_sheet);
  tree.update_style();
  if (showing_) {
    co_await updater.update_task();
    co_await renderer.render();
  }
}

fabric::task<> UIImpl::attach_stylesheet(const std::filesystem::path& style_sheet) {
  co_await fabric::this_task::switch_executor(platform_->get_executor());

  auto& style    = co_await service_context_->require<StyleStore>();
  auto& tree     = co_await service_context_->require<ComponentTree>();
  auto& updater  = co_await service_context_->require<UIUpdater>();
  auto& renderer = co_await service_context_->require<UIRenderer>();

  co_await style.attach_stylesheet(tss::StyleSheet::parse(style_sheet));
  tree.update_style();
  if (showing_) {
    co_await updater.update_task();
    co_await renderer.render();
    platform_->get_bus().emit(RedrawEvent {});
  }
}

fabric::task<> UIImpl::add_style(const std::string& style_string) {
  co_await fabric::this_task::switch_executor(platform_->get_executor());

  auto& style   = co_await service_context_->require<StyleStore>();
  auto& tree    = co_await service_context_->require<ComponentTree>();
  auto& updater = co_await service_context_->require<UIUpdater>();

  co_await style.attach_stylesheet(tss::StyleSheet::parse(style_string));
  tree.update_style();
  if (showing_) {
    updater.schedule_update();
  }
}

fabric::task<> UIImpl::clear_style() {
  co_await fabric::this_task::switch_executor(platform_->get_executor());

  auto& style   = co_await service_context_->require<StyleStore>();
  auto& tree    = co_await service_context_->require<ComponentTree>();
  auto& updater = co_await service_context_->require<UIUpdater>();

  co_await style.clear_style();
  tree.update_style();
  if (showing_) {
    updater.schedule_update();
  }
}
