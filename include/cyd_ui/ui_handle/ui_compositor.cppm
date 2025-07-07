/*! \file  ui_compositor.cppm
 *! \brief
 *!
 */

module;
#include "cyd_ui/debug/profiling/macros.h"
#define PROF_CURRENT_MODULE cydui::layout::ui_compositor

#define GET_VALUE(...)                                                                             \
  dimensions::get_value(__VA_ARGS__).template as<dimensions::screen::pixel>().value
#define GET_IVALUE(...)                                                                            \
  static_cast<int>(                                                                                \
    dimensions::get_value(__VA_ARGS__).template as<dimensions::screen::pixel>().value              \
  )

export module cydui.layout.ui_compositor;
export import cydui.layout.ui_frame;
export import cydui.layout.ui_tree;

import std;
export import reflect;
import cydui.debug.profiling;

import cydui.application;
export import cydui.components.mounted;
import cydui.geometric_relations;

namespace cydui {
  export class ui_compositor {
    ui_frame& frame_;
    ui_tree&  tree_;

  public:
    explicit ui_compositor(
      ui_frame& frame,
      ui_tree&  tree
    )
        : frame_(frame),
          tree_(tree) {}


    void compose() {
      PROF_SCOPE(Compose)
      std::list<compositing::compositing_node_t*> pending_nodes{};
      compose_recurse(*tree_.root, pending_nodes);
      const backends::renderer_base::sptr& renderer = frame_.get_renderer();
      for (const auto& node: pending_nodes) {
        node->compose_into(renderer, tree_.root->get_compositing_node().composite_texture);
      }
      compose_frame();
    }

  private:
    void compose_frame() {
      PROF_SCOPE(Compose Frame)
      auto renderer = frame_.get_renderer();

      renderer->clear();
      renderer->render_texture(tree_.root->get_compositing_node().composite_texture);
      renderer->present();
    }

    void compose_recurse(
      components::mounted_component_t&                   component,
      std::list<compositing::compositing_node_t*>&       pending_nodes,
      const std::optional<backends::texture_base::sptr>& target = std::nullopt
    ) {
      PROF_SCOPE(Compose Recurse)
      auto& node = component.get_compositing_node();

      node.compose_own(frame_.get_frame());
      if (node.is_flattened_node()) {
        for (auto& child: component.get_children()) {
          compose_recurse(*child, pending_nodes, target);
        }
      } else {
        for (auto& child: component.get_children()) {
          compose_recurse(*child, pending_nodes, node.composite_texture);
        }
        if (node.get_operation().position == cydui::position_e::RELATIVE) {
          if (target.has_value()) {
            const backends::renderer_base::sptr& renderer = frame_.get_renderer();
            node.compose_into(renderer, target.value());
          }
        } else {
          pending_nodes.emplace_back(&node);
        }
      }
    }
  };
} // namespace cydui
