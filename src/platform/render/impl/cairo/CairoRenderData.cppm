/*! \file  CairoRenderData.cppm
 *! \brief
 *!
 */

export module cydui.platform.render.cairo.CairoRenderData;

import std;
import reflect;

import fabric.logging;
export import cydui.platform.render.cairo.CairoSurface;

namespace cydui::platform::render {
  export struct CairoRenderData {
    std::optional<CairoSurface> surface{std::nullopt};
  };
} // namespace cydui::platform::render
