/*! \file  WindowFactory.cppm
 *! \brief
 *!
 */

export module cydui.platform.window.window_factory;

import std;
import reflect;

import cydui.platform.window.window_options_base;
import cydui.platform.window.window_base;

import cydui.platform.window.sdl3;
import cydui.platform.window.sdl3.window;

namespace cydui::platform::window {
  export fabric::task<> register_window_service(
    const fabric::services::ServiceContext::sptr& ctx,
    WindowType                                    type,
    const WindowBase::sptr&                       parent,
    const WindowOptionsBase::sptr&                options) {
    if (options->is_type<SDL3WindowOptions>()) {
      co_await ctx->register_service<WindowBase, SDL3Window>(type, parent, options->as<SDL3WindowOptions>());
      co_return;
    }

    throw fabric::exception("Could not create window: unknown backend.");
  }
} // namespace cydui::platform::window
