// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include <tracy/Tracy.hpp>
#include "cyd_fabric_modules/headers/macros/async_events.h"

export module cydui.components.updater;

import std;
import fabric.logging;
import fabric.wiring.signals;

import cydui.application;
import cydui.graphics;
export import cydui.components.base;
export import cydui.components.configure_anchors;

namespace cydui::components {
  export class component_updater_t {
  public:
    using sptr = std::shared_ptr<component_updater_t>;

    static sptr make() {
      return std::make_shared<component_updater_t>();
    }

    fabric::wiring::output_signal<component_updater_t, const component_base_t::sptr&>
      queue_render_signal{};
    fabric::wiring::output_signal<component_updater_t, const component_base_t::sptr&>
      apply_style_signal{};
    fabric::wiring::output_signal<component_updater_t, const component_base_t::sptr&, StyleArchive&>
      compile_style_rules_signal{};

  public:
    void update(component_base_t::sptr component, StyleArchive& style_archive) {
      ZoneScopedN("Update");
      component->state()->_dirty = false;
      queue_render_signal.emit(component);

      // apply_style_signal.emit(component);

      std::unordered_map<
        std::shared_ptr<component_base_t>,
        std::list<std::shared_ptr<component_base_t>>::iterator>
                                                   pending_remove{};
      std::list<std::shared_ptr<component_base_t>> pending_redraw{};
      for (auto it = component->children.begin(); it != component->children.end(); ++it) {
        pending_remove.emplace(*it, it);
      }

      component_builder_t content_children_builder{};
      {
        std::vector<component_builder_t>& content_children = component->attrs()->_content;
        for (auto& item: content_children) {
          for (auto& component: item.get_component_constructors()) {
            content_children_builder.append_component(component);
          }
        }
      }


      std::vector<component_holder_t> new_children =
        component->get_event_dispatcher()->update(style_archive, content_children_builder);

      // Apply style
      apply_style_signal.emit(component);

      add_children(component, new_children, pending_redraw, pending_remove, style_archive);

      // Check if size is fixed
      static const refl::field_info* width_fi =
        refl::type_info::from<components::style_base_t>().field_by_name("width").value();
      static const refl::field_info* height_fi =
        refl::type_info::from<components::style_base_t>().field_by_name("height").value();

      bool fixed_w = component->get_style_data().has_base_field_override(width_fi);
      bool fixed_h = component->get_style_data().has_base_field_override(height_fi);

      auto  dim     = component->get_dimensional_relations();
      auto& int_rel = component->get_internal_relations();

      std::vector<dimensions::dimension<dimensions::screen_measure>>                child_widths{};
      std::unordered_set<dimensions::expression<dimensions::screen_measure>::dep_t> width_deps{};
      std::vector<dimensions::dimension<dimensions::screen_measure>>                child_heights{};
      std::unordered_set<dimensions::expression<dimensions::screen_measure>::dep_t> height_deps{};
      for (auto& child: component->children) {
        auto c_dim = child->get_dimensional_relations();
        child_widths.emplace_back(c_dim.x + c_dim.width);
        width_deps.insert(c_dim.x.as_dependency());
        width_deps.insert(c_dim.width.as_dependency());

        child_heights.emplace_back(c_dim.y + c_dim.height);
        height_deps.insert(c_dim.y.as_dependency());
        height_deps.insert(c_dim.height.as_dependency());
      }

      int_rel.children_total_width = dimensions::function<dimensions::screen_measure>{
        [=] {
          auto max = 0_px;
          for (auto w: child_widths) {
            dimensions::compute(w);
            max = std::max(max, dimensions::get_value(w));
          }
          return max;
        },
        width_deps
      };
      int_rel.children_total_height = dimensions::function<dimensions::screen_measure>{
        [=] {
          auto max = 0_px;
          for (auto h: child_heights) {
            dimensions::compute(h);
            max = std::max(max, dimensions::get_value(h));
          }
          return max;
        },
        height_deps
      };

      if (component->parent.has_value()) {
        auto& parent_int_rel = component->parent.value()->get_internal_relations();

        int_rel.cx = parent_int_rel.cx + dim.x + dim.margin_left + dim.padding_left - dim.scroll_x;
        int_rel.cy = parent_int_rel.cy + dim.y + dim.margin_top + dim.padding_top - dim.scroll_y;
      } else {
        int_rel.cx = dim.x + dim.margin_left + dim.padding_left - dim.scroll_x;
        int_rel.cy = dim.y + dim.margin_top + dim.padding_top - dim.scroll_y;
      }

      if (fixed_w) {
        int_rel.cw = dim.width - dim.padding_left - dim.padding_right;
      } else {
        // If not given, or given has error (ie: circular dep)
        int_rel.cw = int_rel.children_total_width;
        dim.width  = int_rel.cw + dim.padding_left + dim.padding_right;
      }
      if (fixed_h) {
        int_rel.ch = dim.height - dim.padding_top - dim.padding_bottom;
      } else {
        // If not given, or given has error (ie: circular dep)
        int_rel.ch = int_rel.children_total_height;
        dim.height = int_rel.ch + dim.padding_top + dim.padding_bottom;
      }

      for (const auto& remove: pending_remove) {
        dismount_child(component, remove.second);
      }

      for (const auto& child: pending_redraw) {
        // Update children
        update(child, style_archive);
      }
    }

  private:
    void add_children(
      component_base_t::sptr                        component,
      std::vector<component_holder_t>&              children_to_add,
      std::list<std::shared_ptr<component_base_t>>& pending_redraw,
      std::unordered_map<
        std::shared_ptr<component_base_t>,
        std::list<std::shared_ptr<component_base_t>>::iterator>& pending_remove,
      StyleArchive&                                              style_archive
    ) {
      ZoneScopedN("Add Children");
      std::optional<std::shared_ptr<component_base_t>> prev{std::nullopt};

      // Keep track of used IDs just in case some are duplicated.
      // The type is part of the ID, so if there is a mix up there won't be a SEGFAULT
      std::unordered_map<std::string, std::size_t> used_ids{};

      for (auto& item: children_to_add) {
        for (const auto& child: item.get_components()) {
          std::string name = child->name();
          std::string _id  = child->get_id();
          std::string id   = std::format("{}:{}", name, _id);

          if (used_ids.contains(id)) {
            id = std::format("{}[{}]", id, used_ids[id]++);
          } else {
            used_ids[id] = 1;
            id           = std::format("{}[0]", id);
          }

          auto mounted_child =
            mount_child(component, id, child, pending_redraw, pending_remove, style_archive);

          // Configure dimensional context
          anchors::configure_self_anchors(mounted_child);
          anchors::configure_parent_anchors(mounted_child);
          anchors::configure_prev_anchors(mounted_child, prev);

          prev.reset();
          prev.emplace(mounted_child);
        }
      }
    }

    std::shared_ptr<component_base_t> mount_child(
      component_base_t::sptr                        component,
      const std::string&                            id,
      std::shared_ptr<component_base_t>             child,
      std::list<std::shared_ptr<component_base_t>>& pending_redraw,
      std::unordered_map<
        std::shared_ptr<component_base_t>,
        std::list<std::shared_ptr<component_base_t>>::iterator>& pending_remove,
      StyleArchive&                                              style_archive
    ) {
      ZoneScopedN("Mount Children");
      std::shared_ptr<component_base_t> mounted_child{child};
      // Get or Create state for component
      component_state_ref               child_state;
      if (component->state()->children_states.contains(id)) {
        child_state = component->state()->children_states[id];
      } else {
        child_state = component_actor_t::create_state_instance(child.get());
        component->state()->add_children_state(id, child_state);
      }

      if (child_state->component_instance.has_value()) {
        mounted_child = child_state->component_instance.value();
        pending_remove.erase(mounted_child);

        // Redraw child
        if (component_actor_t::update_component_with(mounted_child.get(), child)) {
          pending_redraw.push_back(mounted_child);
        }
      } else {
        // Set child's variables
        mounted_child->parent = component.get();

        component_actor_t::set_component_state(mounted_child.get(), child_state);
        compile_style_rules_signal.emit(mounted_child, style_archive);

        child_state->component_instance = mounted_child;
        component->children.push_back(mounted_child);

        // Configure event handler
        component_actor_t::mount_component(mounted_child.get());

        // Redraw child
        pending_redraw.push_back(mounted_child);
      }

      return mounted_child;
    }

    void dismount_child(
      const component_base_t::sptr&                                 component,
      const std::list<std::shared_ptr<component_base_t>>::iterator& child
    ) {
      ZoneScopedN("Unmount Children");
      component_actor_t::dismount_component(child->get());
      component->children.erase(child);
    }
  };
} // namespace cydui::components
