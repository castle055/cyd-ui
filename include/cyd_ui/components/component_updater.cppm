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

    fabric::wiring::output_signal<component_updater_t, const component_base_t::sptr&> queue_render_signal{};
    fabric::wiring::output_signal<component_updater_t, const component_base_t::sptr&> apply_style_signal{};
    fabric::wiring::output_signal<component_updater_t, const component_base_t::sptr&, StyleArchive&> compile_style_rules_signal{};
  public:
    void update(component_base_t::sptr component, StyleArchive& style_archive) {
      ZoneScopedN("Update");
      component->state()->_dirty     = false;
      queue_render_signal.emit(component);

      // apply_style_signal.emit(component);

      std::unordered_map<
          std::shared_ptr<component_base_t>,
          std::list<std::shared_ptr<component_base_t>>::iterator>
        pending_remove { };
      std::list<std::shared_ptr<component_base_t>> pending_redraw { };
      for (auto it = component->children.begin(); it != component->children.end(); ++it) {
        pending_remove.emplace(*it, it);
      }

      component_builder_t content_children_builder { }; {
        std::vector<component_builder_t> &content_children = component->attrs()->_content;
        for (auto &item: content_children) {
          for (auto &component: item.get_component_constructors()) {
            content_children_builder.append_component(component);
          }
        }
      }


      std::vector<component_holder_t> new_children = component->get_event_dispatcher()->update(style_archive, content_children_builder);

      // Update handler may add to style override
      component->get_style_data().apply_override();

      add_children(component, new_children, pending_redraw, pending_remove, style_archive);

      for (const auto &remove: pending_remove) {
        dismount_child(component, remove.second);
      }

      for (const auto &child: pending_redraw) {
        // Update children
        update(child, style_archive);
      }
    }

  private:
    void add_children(
      component_base_t::sptr component,
      std::vector<component_holder_t> &children_to_add,
      std::list<std::shared_ptr<component_base_t>> &pending_redraw,
      std::unordered_map<
        std::shared_ptr<component_base_t>,
        std::list<std::shared_ptr<component_base_t> >::iterator> &pending_remove,
      StyleArchive &style_archive
    ) {
      ZoneScopedN("Add Children");
      std::optional<std::shared_ptr<component_base_t>> prev {std::nullopt};

      // Keep track of used IDs just in case some are duplicated.
      // The type is part of the ID, so if there is a mix up there won't be a SEGFAULT
      std::unordered_map<std::string, std::size_t> used_ids{};

      for (auto &item: children_to_add) {
        for (const auto &child: item.get_components()) {
          std::string name     = child->name();
          std::string _id = child->get_id();
          std::string id = std::format("{}:{}", name, _id);

          if (used_ids.contains(id)) {
            id = std::format("{}[{}]", id, used_ids[id]++);
          } else {
            used_ids[id] = 1;
            id = std::format("{}[0]", id);
          }

          auto mounted_child = mount_child(component, id, child, pending_redraw, pending_remove, style_archive);
          // Configure dimensional context
          anchors::configure_anchors(mounted_child, prev);

          prev.reset();
          prev.emplace(mounted_child);
        }
      }
    }

    std::shared_ptr<component_base_t> mount_child(
      component_base_t::sptr component,
      const std::string &id,
      std::shared_ptr<component_base_t> child,
      std::list<std::shared_ptr<component_base_t>> &pending_redraw,
      std::unordered_map<
        std::shared_ptr<component_base_t>,
        std::list<std::shared_ptr<component_base_t>>::iterator> &pending_remove,
      StyleArchive &style_archive
    ) {
      ZoneScopedN("Mount Children");
      std::shared_ptr<component_base_t> mounted_child {child};
      // Get or Create state for component
      component_state_ref child_state;
      if (component->state()->children_states.contains(id)) {
        child_state = component->state()->children_states[id];
      } else {
        child_state = component_actor_t::create_state_instance(child.get());
        component->state()->add_children_state(id, child_state);
      }

      if (child_state->component_instance.has_value()) {
        pending_remove.erase(child_state->component_instance.value());
        mounted_child = child_state->component_instance.value();

        // Redraw child
        if (component_actor_t::update_component_with(child_state->component_instance.value().get(), child)) {
          pending_redraw.push_back(child_state->component_instance.value());
        }
      } else {
        // Set child's variables
        child->parent                      = component.get();
        auto c_dims                        = child->get_dimensional_relations();
        child->get_internal_relations().cx = component->get_internal_relations().cx + c_dims.x
                                             + c_dims.margin_left + c_dims.padding_left;
        child->get_internal_relations().cy = component->get_internal_relations().cy + c_dims.y
                                             + c_dims.margin_top + c_dims.padding_top;

        component_actor_t::set_component_state(child.get(), child_state);
        compile_style_rules_signal.emit(child, style_archive);

        child_state->component_instance = child;
        component->children.push_back(child);

        // Configure event handler
        component_actor_t::mount_component(child.get());

        // Apply style
        apply_style_signal.emit(child);

        // Redraw child
        pending_redraw.push_back(child);
      }

      return mounted_child;
    }

    void dismount_child(const component_base_t::sptr& component, const std::list<std::shared_ptr<component_base_t>>::iterator &child) {
      ZoneScopedN("Unmount Children");
      component_actor_t::dismount_component(child->get());
      component->children.erase(child);
    }

  };
}