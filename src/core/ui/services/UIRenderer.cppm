/*! \file  UIRenderer.cppm
 *! \brief
 *!
 */
module;
#include "../../../debug/profiling/macros.h"
#define PROF_CURRENT_MODULE cydui::core::ui::services::UIRenderer

#include <tracy/Tracy.hpp>

export module cydui.core.ui.services.UIRenderer;
export import cydui.core.ui.services.PlatformController;
export import cydui.core.ui.services.ComponentTree;

import std;
export import reflect;
import cydui.debug.profiling;

import cydui.platform.render.renderer_base;

namespace cydui::detail::ui::services {
  export class UIRenderer final: public fabric::services::ServiceBase {
    PlatformController& platform_;
    ComponentTree&      tree_;

    UIRenderer(
      PlatformController& frame,
      ComponentTree&      tree)
        : platform_(frame),
          tree_(tree) {}

  public:
    static fabric::task<sptr> start(fabric::services::ServiceLocator& locator) {
      auto& platform = co_await locator.require<PlatformController>();
      auto& tree     = co_await locator.require<ComponentTree>();

      co_return sptr {new UIRenderer(platform, tree)};
    }

    fabric::task<> render() {
      PROF_SCOPE(Render Flow)

      platform::render::FrameRendererBase::uptr current_frame = co_await platform_.begin_frame();

      {
        PROF_SCOPE(Render)
        co_await current_frame->render(tree_.get_root());
      }

      platform::Surface surface {};
      {
        PROF_SCOPE(Finish)
        surface = co_await current_frame->finish();
      }

      {
        PROF_SCOPE(Present)
        co_await platform_.present(surface);
      }
    }
  };
} // namespace cydui::detail::ui::services
