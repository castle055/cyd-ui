/*! \file  ui_renderer.cppm
 *! \brief
 *!
 */
module;
#include "cyd_ui/debug/profiling/macros.h"
#define PROF_CURRENT_MODULE cydui::layout::ui_renderer

#include <tracy/Tracy.hpp>

export module cydui.layout.ui_renderer;
export import cydui.layout.ui_frame;
export import cydui.layout.ui_tree;
export import cydui.layout.ui_compositor;

import std;
export import reflect;
import cydui.debug.profiling;

import cydui.application;
export import cydui.components.mounted;

namespace cydui {
  export class ui_renderer {
    ui_frame&      frame_;
    ui_tree&       tree_;
    ui_compositor& compositor_;

  public:
    ui_renderer(
      ui_frame&      frame,
      ui_tree&       tree,
      ui_compositor& compositor
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

    void begin_component_render_recurse(components::mounted_component_t& component) {
      PROF_SCOPE(Begin Render Recurse)
      if (component.get_compositing_node().is_out_of_bounds()) {
        return;
      }

      component.get_compositing_node().begin_render(frame_.get_frame());

      for (auto& child: component.get_children()) {
        begin_component_render_recurse(*child);
      }
    }

    void end_component_render_recurse(components::mounted_component_t& component) {
      PROF_SCOPE(End Render Recurse)
      if (component.get_compositing_node().is_out_of_bounds()) {
        return;
      }

      component.get_compositing_node().end_render();

      for (auto& child: component.get_children()) {
        end_component_render_recurse(*child);
      }
    }

    void repaint_recurse(components::mounted_component_t& component) {
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
