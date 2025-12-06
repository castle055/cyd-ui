/*! \file  UIImpl.cppm
 *! \brief
 *!
 */

module;
#include <tracy/Tracy.hpp>

export module cydui.ui_handle.impl;

import std;
import reflect;
import fabric.logging;
import fabric.profiling;

export import cydui.dimensions;
export import cydui.styling.lang;

export import cydui.core.blueprint;
export import cydui.ui_handle;
export import cydui.ui_options;
export import cydui.platform.impl;

import cydui.geometry;

namespace cydui {
  export class UIImpl final: public UI {
    platform::PlatformImpl::sptr                  platform_;
    fabric::services::ServiceContext::sptr        service_context_;
    fabric::services::ServiceContext::sptr        ui_service_context_;
    bool                                          showing_ {false};
    fabric::async::listener<WindowCloseRequested> window_closed_listener_;

  public:
    using sptr = std::shared_ptr<UIImpl>;

    UIImpl(
      platform::PlatformImpl::sptr                  platform,
      const fabric::services::ServiceContext::sptr& service_context,
      const fabric::services::ServiceContext::sptr& ui_service_context);

    ~UIImpl() override;

    static fabric::task<sptr> make(
      platform::PlatformImpl::sptr platform,
      Blueprint::uptr              root,
      const UIOptions&             options);

    static fabric::task<sptr> make_as_child(
      const UIImpl&                                      parent,
      platform::window::WindowType                       type,
      Blueprint::uptr                                    root,
      const platform::window::WindowOptionsBase::sptr&   window_options,
      const platform::render::RendererOptionsBase::sptr& render_options,
      const UIOptions&                                   options);


    fabric::task<UI::sptr> make_child_ui_impl(
      Blueprint::uptr                                    root,
      platform::window::WindowType                       type,
      const platform::window::WindowOptionsBase::sptr&   window_options,
      const platform::render::RendererOptionsBase::sptr& render_options,
      const UIOptions&                                   options) override;

    fabric::task<> attach_stylesheet(const tss::StyleSheet::sptr& style_sheet) override;

    fabric::task<> attach_stylesheet(const std::filesystem::path& style_sheet) override;

    fabric::task<> add_style(const std::string& style_string) override;

    fabric::task<> clear_style() override;

    fabric::task<> show() override;

    fabric::task<> until_closed() override;

    fabric::async::async_bus_t& bus() override;
  };
} // namespace cydui
