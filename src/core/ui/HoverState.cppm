/*! \file  HoverState.cppm
 *! \brief
 *!
 */

module;
#include "../../debug/profiling/macros.h"
#define PROF_CURRENT_MODULE cydui::core::hover_state

export module cydui.core.hover_state;
export import cydui.core.style;
export import cydui.core.tree;

import std;
export import reflect;
import cydui.debug.profiling;

namespace cydui::core {
  export class HoverState {
    StyleStore& style_;
    ComponentTree&  tree_;

    dimensions::screen_measure mouse_x{-1_px};
    dimensions::screen_measure mouse_y{-1_px};

  public:
    explicit HoverState(
      StyleStore& style,
      ComponentTree&  tree
    )
        : style_(style),
          tree_(tree) {}

    void update_mouse_position(
      const dimensions::screen_measure& mouse_x,
      const dimensions::screen_measure& mouse_y
    ) {
      this->mouse_x = mouse_x;
      this->mouse_y = mouse_y;
    }

    bool update_hover() {
      if (mouse_x <= -1_px or mouse_y <= -1_px) {
        clear_hovering_flag(*tree_.root, mouse_x, mouse_y);
        return true;
      }

      mounted_component_t* specified_target = tree_.find_by_coords(mouse_x, mouse_y);
      if (specified_target != nullptr) {
        return set_hovering_flag(*specified_target, mouse_x, mouse_y, true);
      }

      return false;
    }

    bool set_hovering_flag(
      mounted_component_t&  component,
      const dimensions::screen_measure& mouse_x,
      const dimensions::screen_measure& mouse_y,
      bool                              clear_children
    ) {
      PROF_SCOPE(set_hovering_flag)
      if (clear_children) {
        for (const auto& child: component.get_children()) {
          clear_hovering_flag(*child, mouse_x, mouse_y);
        }
      }

      if (not component.is_hovered()) {
        component.set_hovered(true);

        auto& geom          = component.get_geometry();
        auto [rel_x, rel_y] = geom.get_relative(mouse_x, mouse_y);
        component.get_event_dispatcher().dispatch_mouse_enter(rel_x, rel_y);

        component.get_style_stack().mark_dirty();
        component.mark_dirty();

        if (not component.is_root()) {
          auto parent = component.get_parent();
          if (parent->is_hovered()) {
            for (auto& child: parent->get_children()) {
              if (child.get() != &component) {
                clear_hovering_flag(*child, mouse_x, mouse_y);
              }
            }
          } else {
            set_hovering_flag(*parent, mouse_x, mouse_y, false);
          }
        }

        return true;
      }

      return false;
    }

    void clear_hovering_flag(
      mounted_component_t&  component,
      const dimensions::screen_measure& mouse_x,
      const dimensions::screen_measure& mouse_y
    ) {
      PROF_SCOPE(clear_hovering_flag)
      if (not component.is_hovered()) {
        return;
      }
      component.set_hovered(false);

      auto& geom          = component.get_geometry();
      auto [rel_x, rel_y] = geom.get_relative(mouse_x, mouse_y);
      component.get_event_dispatcher().dispatch_mouse_exit(rel_x, rel_y);

      component.get_style_stack().mark_dirty();
      component.mark_dirty();

      for (const auto& child: component.get_children()) {
        clear_hovering_flag(*child, mouse_x, mouse_y);
      }
    }
  };
} // namespace cydui
