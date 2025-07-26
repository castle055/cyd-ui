/*! \file  ui_renderer.cppm
 *! \brief
 *!
 */
module;
#include "../../debug/profiling/macros.h"
#define PROF_CURRENT_MODULE cydui::core::renderer

#include <tracy/Tracy.hpp>

export module cydui.core.renderer;
export import cydui.core.frame_controller;
export import cydui.core.tree;
export import cydui.core.compositor;

import std;
export import reflect;
import cydui.debug.profiling;

import cydui.application;
export import cydui.core.mounted;

namespace cydui::core {
  export class UIRenderer {
    FrameController&      frame_;
    ComponentTree&       tree_;
    UICompositor& compositor_;

  public:
    UIRenderer(
      FrameController&      frame,
      ComponentTree&       tree,
      UICompositor& compositor
    )
        : frame_(frame),
          tree_(tree),
          compositor_(compositor) {}

    void render() {
      PROF_SCOPE(Render Flow)
      Application::run([&]() { begin_render(); });

      {
        PROF_SCOPE(Repaint)
        repaint_recurse(*tree_.root);
      }

      Application::run([&]() {
        end_render();
        compositor_.compose();
      });
    }

  private:
    void begin_render() {
      PROF_SCOPE(Begin Render)
      begin_component_render_recurse(*tree_.root);
    }

    void end_render() {
      PROF_SCOPE(End Render)
      end_component_render_recurse(*tree_.root);
    }

    void begin_component_render_recurse(mounted_component_t& component) {
      PROF_SCOPE(Begin Render Recurse)
      if (component.get_compositing_node().is_out_of_bounds()) {
        return;
      }

      component.get_compositing_node().begin_render(frame_.get_frame());

      for (auto& child: component.get_children()) {
        begin_component_render_recurse(*child);
      }
    }

    void end_component_render_recurse(mounted_component_t& component) {
      PROF_SCOPE(End Render Recurse)
      if (component.get_compositing_node().is_out_of_bounds()) {
        return;
      }

      component.get_compositing_node().end_render();

      for (auto& child: component.get_children()) {
        end_component_render_recurse(*child);
      }
    }

    void repaint_recurse(mounted_component_t& component) {
      PROF_SCOPE(Repaint Recurse)
      auto& node = component.get_compositing_node();
      if (node.is_out_of_bounds()) {
        return;
      }

      node.render();

      for (auto& child: component.get_children()) {
        repaint_recurse(*child);
      }
    }
  };
} // namespace cydui
