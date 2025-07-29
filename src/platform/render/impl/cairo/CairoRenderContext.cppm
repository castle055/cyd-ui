/*! \file  CairoRenderContext.cppm
 *! \brief
 *!
 */

export module cydui.platform.render.cairo.CairoRenderContext;

import std;
import reflect;

import fabric.logging;
export import cydui.platform.render.cairo.SwapPtr;
export import cydui.platform.render.cairo.CairoSurface;
export import cydui.platform.render.cairo;

namespace cydui::platform::render {
  export class CairoRenderContext {
    SwapPtr<CairoSurface> swap_buffer_;

  public:
    using sptr = std::shared_ptr<CairoRenderContext>;

    explicit CairoRenderContext(const CairoRendererOptions& options)
        : swap_buffer_(
            1,
            1
          ) {}

    SwapPtr<CairoSurface>& get_swap_buffer() {
      return swap_buffer_;
    }
  };
} // namespace cydui::platform::render
