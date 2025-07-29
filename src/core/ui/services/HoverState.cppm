/*! \file  HoverState.cppm
 *! \brief
 *!
 */

module;
#include "../../../debug/profiling/macros.h"
#define PROF_CURRENT_MODULE cydui::core::ui::services::HoverState

export module cydui.core.ui.services.HoverState;
export import cydui.core.ui.services.StyleStore;
export import cydui.core.ui.services.ComponentTree;

import std;
export import reflect;
import cydui.debug.profiling;

namespace cydui::detail::ui::services {
  export class HoverState final: public fabric::services::ServiceBase {
    StyleStore&    style_;
    ComponentTree& tree_;

    dimensions::screen_measure mouse_x {-1_px};
    dimensions::screen_measure mouse_y {-1_px};

    HoverState(
      StyleStore&    style,
      ComponentTree& tree)
        : style_(style),
          tree_(tree) {}

  public:
    static fabric::task<sptr> start(fabric::services::ServiceLocator& locator) {
      auto& style_store = co_await locator.require<StyleStore>();
      auto& tree        = co_await locator.require<ComponentTree>();

      co_return sptr {new HoverState(style_store, tree)};
    }

    void update_mouse_position(
      const dimensions::screen_measure& mouse_x,
      const dimensions::screen_measure& mouse_y) {
      this->mouse_x = mouse_x;
      this->mouse_y = mouse_y;
    }

    bool update_hover() {
      if (mouse_x <= -1_px or mouse_y <= -1_px) {
        clear_hovering_flag(tree_.get_root(), mouse_x, mouse_y);
        return true;
      }

      ComponentImpl& specified_target = tree_.find_by_coords(mouse_x, mouse_y);
      return set_hovering_flag(specified_target, mouse_x, mouse_y, true);
    }

    bool set_hovering_flag(
      ComponentImpl&                    component,
      const dimensions::screen_measure& mouse_x,
      const dimensions::screen_measure& mouse_y,
      bool                              clear_children) {
      PROF_SCOPE(set_hovering_flag)
      if (clear_children) {
        for (const auto& child: component.get_children_impl()) {
          clear_hovering_flag(*child, mouse_x, mouse_y);
        }
      }

      if (not component.is_hovered()) {
        component.set_hovered(true);

        auto& geom          = component.get_geometry();
        auto [rel_x, rel_y] = geom.get_relative(mouse_x, mouse_y);
        component.get_event_dispatcher().dispatch_mouse_enter(rel_x, rel_y);

        style_.update_style(component);
        component.mark_dirty();

        if (not component.is_root()) {
          auto parent = component.get_parent_impl();
          if (parent->is_hovered()) {
            for (auto& child: parent->get_children_impl()) {
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
      ComponentImpl&                    component,
      const dimensions::screen_measure& mouse_x,
      const dimensions::screen_measure& mouse_y) {
      PROF_SCOPE(clear_hovering_flag)
      if (not component.is_hovered()) {
        return;
      }
      component.set_hovered(false);

      auto& geom          = component.get_geometry();
      auto [rel_x, rel_y] = geom.get_relative(mouse_x, mouse_y);
      component.get_event_dispatcher().dispatch_mouse_exit(rel_x, rel_y);

      style_.update_style(component);
      component.mark_dirty();

      for (const auto& child: component.get_children_impl()) {
        clear_hovering_flag(*child, mouse_x, mouse_y);
      }
    }
  };
} // namespace cydui::detail::ui::services
