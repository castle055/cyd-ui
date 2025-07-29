/*! \file  CairoRenderer.cppm
 *! \brief
 *!
 */

module;
#include <pangomm/init.h>
export module cydui.platform.render.cairo.renderer;

import std;
import reflect;

import fabric.logging;
export import cydui.platform.render.renderer_base;
export import cydui.platform.render.cairo;
export import cydui.platform.render.cairo.CairoRenderContext;
export import cydui.platform.render.cairo.CairoRenderData;
export import cydui.platform.render.cairo.CairoFrameRenderer;

namespace cydui::platform::render {
  export class CairoRenderer final: public RendererBase {
    CairoRenderContext::sptr context_;

    explicit CairoRenderer(const CairoRendererOptions& options)
        : context_(std::make_shared<CairoRenderContext>(options)) {}

  public:
    using sptr = std::shared_ptr<CairoRenderer>;

    static fabric::task<sptr> start(
      fabric::services::ServiceLocator& locator,
      const CairoRendererOptions&       options) {
      Pango::init();
      co_return sptr {new CairoRenderer(options)};
    }

    CairoRenderer(const CairoRenderer&)            = delete;
    CairoRenderer& operator=(const CairoRenderer&) = delete;
    CairoRenderer(CairoRenderer&&)                 = delete;
    CairoRenderer& operator=(CairoRenderer&&)      = delete;

    fabric::task<FrameRendererBase::uptr> begin_frame(
      int width,
      int height) override {
      co_return std::make_unique<CairoFrameRenderer>(context_, width, height);
    }

    std::optional<refl::any> mount_component(detail::ComponentImpl& component) override {
      return CairoRenderData {};
    }

    void unmount_component(detail::ComponentImpl& component) override {
      return;
    }
  };
} // namespace cydui::platform::render
