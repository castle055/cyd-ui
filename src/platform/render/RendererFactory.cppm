/*! \file  RendererFactory.cppm
 *! \brief
 *!
 */

export module cydui.platform.render.renderer_factory;

import std;
import reflect;

import cydui.platform.render.renderer_options_base;
import cydui.platform.render.renderer_base;

import cydui.platform.render.cairo;
import cydui.platform.render.cairo.renderer;

namespace cydui::platform::render {
  export fabric::task<> register_render_service(
    const fabric::services::ServiceContext::sptr& ctx,
    const RendererOptionsBase::sptr&              options
  ) {
    if (options->is_type<CairoRendererOptions>()) {
      co_await ctx->register_service<RendererBase, CairoRenderer>(options->as<CairoRendererOptions>(
      ));
      co_return;
    }

    throw std::runtime_error("Could not create window: unknown backend.");
  }
} // namespace cydui::platform::render
