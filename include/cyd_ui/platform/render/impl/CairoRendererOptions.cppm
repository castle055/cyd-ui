/*! \file  CairoRendererOptions.cppm
 *! \brief
 *!
 */

export module cydui.platform.render.cairo;

import std;
import reflect;

export import cydui.platform.render.renderer_options_base;

namespace cydui::platform::render {
  export class CairoRendererOptions final: public RendererOptionsBase {
  public:
    CairoRendererOptions()
        : RendererOptionsBase(refl::type_id<CairoRendererOptions>) {}
  };
} // namespace cydui::platform::render
