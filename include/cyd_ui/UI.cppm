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

    virtual fabric::task<> attach_stylesheet(const tss::StyleSheet::sptr& style_sheet) = 0;
    virtual fabric::task<> attach_stylesheet(const std::filesystem::path& style_sheet) = 0;
    virtual fabric::task<> add_style(const std::string& style_string)                  = 0;
    virtual fabric::task<> clear_style()                                               = 0;
    virtual fabric::task<> show()                                                      = 0;
    virtual fabric::task<> until_closed()                                              = 0;
    virtual fabric::async::async_bus_t& bus()                                          = 0;
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
