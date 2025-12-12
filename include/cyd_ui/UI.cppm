/*! \file  UI.cppm
 *! \brief
 *!
 */

module;
#include <tracy/Tracy.hpp>

export module cydui.ui_handle;

import std;
import reflect;
import fabric.logging;
import fabric.profiling;

export import cydui.styling.lang;

export import cydui.core.blueprint;
export import cydui.ui_options;
export import cydui.platform;

import cydui.geometry;

namespace cydui {
  export class UI {
  public:
    using sptr = std::shared_ptr<UI>;

    virtual ~UI() = default;

    virtual fabric::task<>                    attach_stylesheet(const tss::StyleSheet::sptr& style_sheet) = 0;
    virtual fabric::task<>                    attach_stylesheet(const std::filesystem::path& style_sheet) = 0;
    virtual fabric::task<>                    add_style(const std::string& style_string)                  = 0;
    virtual fabric::task<>                    clear_style()                                               = 0;
    virtual fabric::task<>                    show()                                                      = 0;
    virtual fabric::task<>                    until_closed()                                              = 0;
    virtual fabric::async::async_bus_t&       bus()                                                       = 0;
    virtual fabric::services::ServiceContext& service_context()                                           = 0;

    template <ComponentBlueprint C>
    fabric::task<UI::sptr> make_child_ui(
      platform::window::WindowType                         type,
      const C&                                             root_component,
      const platform::window::WindowOptionsConcept auto&   window_options,
      const platform::render::RendererOptionsConcept auto& renderer_options,
      UIOptions                                            options = {}) {
      std::unique_ptr<C>                        root_blueprint = std::make_unique<C>(root_component);
      platform::window::WindowOptionsBase::sptr window_options_ =
        std::make_shared<std::remove_cvref_t<decltype(window_options)>>(window_options);
      platform::render::RendererOptionsBase::sptr render_options_ =
        std::make_shared<std::remove_cvref_t<decltype(renderer_options)>>(renderer_options);
      co_return co_await make_child_ui_impl(std::move(root_blueprint), type, window_options_, render_options_, options);
    }

  private:
    virtual fabric::task<UI::sptr> make_child_ui_impl(
      Blueprint::uptr                                    root,
      platform::window::WindowType                       type,
      const platform::window::WindowOptionsBase::sptr&   window_options,
      const platform::render::RendererOptionsBase::sptr& render_options,
      const UIOptions&                                   options) = 0;
  };

  fabric::task<UI::sptr> make_impl(
    Platform::sptr   platform,
    Blueprint::uptr  root,
    const UIOptions& options);

  export template <ComponentBlueprint C>
  fabric::task<UI::sptr> make_ui(
    Platform::sptr platform,
    const C&       root_component,
    UIOptions      options = {}) {
    std::unique_ptr<C> root_blueprint = std::make_unique<C>(root_component);
    co_return co_await make_impl(platform, std::move(root_blueprint), options);
  }
} // namespace cydui
