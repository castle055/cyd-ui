/*! \file  ComponentTree.cppm
 *! \brief
 *!
 */

module;
#include "../../debug/profiling/macros.h"
#define PROF_CURRENT_MODULE cydui::core::tree

#include <tracy/Tracy.hpp>

export module cydui.core.tree;
export import cydui.core.style;
export import cydui.core.focus_state;

import std;
export import reflect;
import cydui.debug.profiling;

export import cydui.core.state;
export import cydui.core.mounted;
import cydui.geometric_relations;
import cydui.geometry.configure_anchors;
import cydui.animations;

namespace cydui::core {
  export class ComponentTree {
    StyleStore&    style_;
    FocusState& focus_state_;

  public:
    component_state_t::sptr   root_state;
    mounted_component_t::uptr root;

    explicit ComponentTree(
      StyleStore&    style,
      FocusState& focus_state
    )
        : style_(style),
          focus_state_(focus_state) {}

    mounted_component_t* find_by_coords(
      dimensions::screen_measure x,
      dimensions::screen_measure y
    ) {
      PROF_SCOPE(Find by coords)
      PROF_SCOPE_TEXT("X: {}", x.to_string());
      PROF_SCOPE_TEXT("Y: {}", y.to_string());
      return layout::find_by_coords(*root, x, y);
    }

    void update_children(
      mounted_component_t&             component,
      content_type&                    children_to_add,
      std::list<mounted_component_t*>& pending_redraw
    ) {
      PROF_SCOPE(Update Children)
      std::optional<mounted_component_t*> prev{std::nullopt};

      std::unordered_map<
        mounted_component_t*,
        std::list<mounted_component_t::uptr>::iterator>
        pending_remove{};
      for (auto it = component.get_children().begin(); it != component.get_children().end(); ++it) {
        pending_remove.emplace(it->get(), it);
      }

      for (auto& child: children_to_add) {
        const auto& style_override     = child->get_style_override();
        auto        overriden_position = style_override.get_field("position");

        auto mounted_child =
          mount_child(component, std::move(child), pending_redraw, pending_remove);

        // Configure dimensional context
        layout::anchors::configure_self_anchors(*mounted_child);
        layout::anchors::configure_parent_anchors(*mounted_child);
        layout::anchors::configure_prev_anchors(*mounted_child, prev);

        const auto& style = mounted_child->get_style();

        bool position_is_relative =
          (overriden_position.has_value() ? overriden_position.value().as<position_e>()
                                          : style.position)
          == position_e::RELATIVE;

        if (position_is_relative) {
          prev = mounted_child;
        }
      }

      for (const auto& remove: pending_remove) {
        dismount_child(component, remove.second);
      }
    }

    void set_root_size(
      dimensions::screen_measure w,
      dimensions::screen_measure h
    ) {
      PROF_SCOPE(Set Root Size)
      PROF_SCOPE_TEXT("Width: {}", w.to_string());
      PROF_SCOPE_TEXT("Height: {}", h.to_string());

      root->get_blueprint()->width(w);
      root->get_blueprint()->height(h);
      root->mark_dirty();
    }

  private:
    mounted_component_t* mount_child(
      mounted_component_t&             component,
      blueprint_base_t::uptr           child,
      std::list<mounted_component_t*>& pending_redraw,
      std::unordered_map<
        mounted_component_t*,
        std::list<mounted_component_t::uptr>::iterator>& pending_remove
    ) {
      PROF_SCOPE(Mount Child)
      mounted_component_t*    mounted_child;
      std::string                         id = child->get_id().str();
      // Get or Create state for component
      component_state_t::sptr child_state;
      const auto&                         parent_state = component.get_state();
      if (parent_state->children_states.contains(id)) {
        child_state = parent_state->children_states[id];
      } else {
        child_state = child->make_state_object();
        parent_state->add_children_state(id, child_state);
      }

      // Update existing component or mount new one
      auto existing_component = component.find_child(id);
      if (existing_component.has_value()) {
        mounted_child = existing_component.value();
        pending_remove.erase(mounted_child);

        if (mounted_child->update_with(*child)) {
          mounted_child->mark_dirty();
          pending_redraw.push_back(mounted_child);
        }
      } else {
        mounted_child =
          mounted_component_t::make(component, std::move(child), child_state);
        PROF_MESSAGE("Component Mounted ({})", mounted_child->get_name());

        style_.compile_rules(*mounted_child);

        mounted_child->get_event_dispatcher().dispatch_mount(
          mounted_child->get_blueprint()->get_content()
        );

        pending_redraw.push_back(mounted_child);
      }

      return mounted_child;
    }

    void dismount_child(
      mounted_component_t&                                  component,
      const std::list<mounted_component_t::uptr>::iterator& child
    ) {
      ZoneScopedN("Dismount Child");

      const auto& win              = component.get_window();
      auto&       resource_context = *win->get_executor()->get_spawn_context();
      auto&       anim_sys         = *resource_context.get_resource<animations::AnimationSystem>();
      anim_sys.clear_animations_for_component(*child->get());

      auto focused = focus_state_.get_focused();
      if (focused.has_value() and focused.value() == child->get()) {
        focus_state_.unfocus();
      }

      PROF_MESSAGE("Component Dismounted ({})", (*child)->get_name());
      component.get_children().erase(child);
    }
  };
} // namespace cydui
