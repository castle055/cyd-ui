/*! \file  ui_tree.cppm
 *! \brief
 *!
 */

module;
#include "cyd_ui/debug/profiling/macros.h"
#define PROF_CURRENT_MODULE cydui::layout::ui_tree

#include <cstddef>
#include <tracy/Tracy.hpp>

export module cydui.layout.ui_tree;
export import cydui.layout.ui_style;
export import cydui.layout.focus_state;

import std;
export import reflect;
import cydui.debug.profiling;

export import cydui.components.state;
export import cydui.components.mounted;
import cydui.geometric_relations;
import cydui.geometry.configure_anchors;
import cydui.animations;

namespace cydui {
  export class ui_tree {
    ui_style&    style_;
    focus_state& focus_state_;

  public:
    components::component_state_t::sptr   root_state;
    components::mounted_component_t::uptr root;

    explicit ui_tree(
      ui_style&    style,
      focus_state& focus_state
    )
        : style_(style),
          focus_state_(focus_state) {}

    components::mounted_component_t* find_by_coords(
      dimensions::screen_measure x,
      dimensions::screen_measure y
    ) {
      PROF_SCOPE(Find by coords)
      PROF_SCOPE_TEXT("X: {}", x.to_string());
      PROF_SCOPE_TEXT("Y: {}", y.to_string());
      return geometry::find_by_coords(*root, x, y);
    }

    void update_children(
      components::mounted_component_t&             component,
      components::content_type&                    children_to_add,
      std::list<components::mounted_component_t*>& pending_redraw
    ) {
      PROF_SCOPE(Update Children)
      std::optional<components::mounted_component_t*> prev{std::nullopt};

      std::unordered_map<
        components::mounted_component_t*,
        std::list<components::mounted_component_t::uptr>::iterator>
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
        geometry::anchors::configure_self_anchors(*mounted_child);
        geometry::anchors::configure_parent_anchors(*mounted_child);
        geometry::anchors::configure_prev_anchors(*mounted_child, prev);

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
    components::mounted_component_t* mount_child(
      components::mounted_component_t&             component,
      components::blueprint_base_t::uptr           child,
      std::list<components::mounted_component_t*>& pending_redraw,
      std::unordered_map<
        components::mounted_component_t*,
        std::list<components::mounted_component_t::uptr>::iterator>& pending_remove
    ) {
      PROF_SCOPE(Mount Child)
      components::mounted_component_t*    mounted_child;
      std::string                         id = child->get_id().str();
      // Get or Create state for component
      components::component_state_t::sptr child_state;
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
          components::mounted_component_t::make(component, std::move(child), child_state);
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
      components::mounted_component_t&                                  component,
      const std::list<components::mounted_component_t::uptr>::iterator& child
    ) {
      ZoneScopedN("Dismount Child");

      const auto& win              = component.get_window();
      auto&       resource_context = *win->get_executor()->get_spawn_context();
      auto&       anim_sys         = *resource_context.get_resource<AnimationSystem>();
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
