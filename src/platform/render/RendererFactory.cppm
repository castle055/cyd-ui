/*! \file  RendererFactory.cppm
 *! \brief
 *!
 */

export module cydui.platform.render.renderer_factory;

import std;
import reflect;

import cydui.platform.render.renderer_options_base;
import cydui.platform.render.renderer_base;

namespace cydui::platform::render {
  export RendererBase::sptr make_renderer(const RendererOptionsBase::sptr& options) {}
} // namespace cydui::platform::render
