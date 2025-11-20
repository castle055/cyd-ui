/*! \file  ComponentTree.cppm
 *! \brief
 *!
 */

module;
#include "../../../debug/profiling/macros.h"
#define PROF_CURRENT_MODULE cydui::core::ui::services::ComponentTree

#include <tracy/Tracy.hpp>

export module cydui.core.ui.services.ComponentTree;
export import cydui.core.ui.services.StyleStore;
export import cydui.core.ui.services.FocusState;
export import cydui.core.ui.services.PlatformController;

import std;
export import reflect;
import cydui.debug.profiling;

export import cydui.core.state;
export import cydui.core.Component.impl;
import cydui.geometric_relations;
import cydui.geometry.configure_anchors;

namespace cydui::detail::ui::services {
  export class ComponentTree final: public fabric::services::ServiceBase {
    StyleStore&         style_;
    FocusState&         focus_state_;
    PlatformController& platform_;

    ComponentImpl::uptr  root;
    ComponentState::sptr root_state;

    explicit ComponentTree(
      StyleStore&         style,
      FocusState&         focus_state,
      PlatformController& frame_controller,
      ComponentImpl::uptr root)
        : style_(style),
          focus_state_(focus_state),
          platform_(frame_controller),
          root(std::move(root)),
          root_state(this->root->get_state()) {}

  public:
    static fabric::task<sptr> start(
      fabric::services::ServiceLocator& locator,
      ComponentImpl::uptr*              root) {
      auto& platform    = co_await locator.require<PlatformController>();
      auto& style       = co_await locator.require<StyleStore>();
      auto& focus_state = co_await locator.require<FocusState>();

      auto ptr = sptr {new ComponentTree(style, focus_state, platform, std::move(*root))};

      co_return ptr;
    }

    void update_style() {
      style_.compile_rules(*root);
      style_.update_style(*root);
      root->mark_dirty();
    }

    ComponentImpl& find_by_coords(
      dimensions::screen_measure x,
      dimensions::screen_measure y) {
      PROF_SCOPE(Find by coords)
      PROF_SCOPE_TEXT("X: {}", x.to_string());
      PROF_SCOPE_TEXT("Y: {}", y.to_string());
      auto ptr = layout::find_by_coords(*root, x, y);
      return ptr == nullptr ? *root : *ptr;
    }

    void update_children(
      ComponentImpl&             component,
      BlueprintList&             children_to_add,
      std::list<ComponentImpl*>& pending_redraw) {
      PROF_SCOPE(Update Children)

      std::unordered_map<ComponentImpl*, std::list<ComponentImpl::uptr>::iterator> pending_remove {};
      for (auto it = component.get_children_impl().begin(); it != component.get_children_impl().end(); ++it) {
        pending_remove.emplace(it->get(), it);
      }

      std::size_t modified_children {0};

      for (auto& child: children_to_add) {
        mount_child(component, std::move(child), pending_redraw, pending_remove);
        modified_children++;
      }

      for (const auto& remove: pending_remove) {
        unmount_child(component, remove.second);
        modified_children++;
      }

      if (modified_children > 0) {
        std::optional<ComponentImpl*> prev {std::nullopt};
        for (auto& child: component.get_children_impl()) {
          // Configure dimensional context
          layout::anchors::configure_anchors(*child, prev);

          const auto& style              = child->get_style();
          const auto& style_override     = child->get_blueprint().get_style_override();
          auto        overriden_position = style_override.get_field("position");

          bool position_is_relative =
            (overriden_position.has_value() ? overriden_position.value().as<position_e>() : style.position)
            == position_e::RELATIVE;

          if (position_is_relative) {
            prev = child.get();
          }
        }
      }
    }

    void update_root_size() {
      PROF_SCOPE(Update Root Size)
      auto [w, h] = platform_.get_size();

      auto& geom = root->get_geometry();
      geom.set_position_absolute(layout::X_AXIS);
      geom.set_position_absolute(layout::Y_AXIS);
      geom.set_sizing_mode(layout::X_AXIS, layout::component_sizing::FIXED);
      geom.set_sizing_mode(layout::Y_AXIS, layout::component_sizing::FIXED);
      // PROF_SCOPE_TEXT("Width: {}", w.to_string());
      // PROF_SCOPE_TEXT("Height: {}", h.to_string());

      root->get_blueprint().width(w);
      root->get_blueprint().height(h);
      root->mark_dirty();
    }

    void update_root_size(
      dimensions::screen_measure w,
      dimensions::screen_measure h) {
      PROF_SCOPE(Update Root Size)
      auto& geom = root->get_geometry();
      geom.set_position_absolute(layout::X_AXIS);
      geom.set_position_absolute(layout::Y_AXIS);
      geom.set_sizing_mode(layout::X_AXIS, layout::component_sizing::FIXED);
      geom.set_sizing_mode(layout::Y_AXIS, layout::component_sizing::FIXED);
      // PROF_SCOPE_TEXT("Width: {}", w.to_string());
      // PROF_SCOPE_TEXT("Height: {}", h.to_string());

      root->get_blueprint().width(w);
      root->get_blueprint().height(h);
      root->mark_dirty();
    }

    ComponentImpl& get_root() {
      return *root;
    }

  private:
    ComponentImpl* mount_child(
      ComponentImpl&             component,
      Blueprint::uptr            child,
      std::list<ComponentImpl*>& pending_redraw,
      std::unordered_map<
        ComponentImpl*,
        std::list<ComponentImpl::uptr>::iterator>& pending_remove) {
      PROF_SCOPE(Mount Child)
      ComponentImpl*       mounted_child;
      std::string          id = child->get_id().str();
      // Get or Create state for component
      ComponentState::sptr child_state;
      const auto&          parent_state = component.get_state();
      if (parent_state->children_states.contains(id)) {
        child_state = parent_state->children_states[id];
      } else {
        child_state = child->make_state_object();
        parent_state->add_children_state(id, child_state);
      }

      // Update existing component or mount new one
      auto existing_component = component.find_child_impl(id);
      if (existing_component.has_value()) {
        mounted_child = existing_component.value();
        pending_remove.erase(mounted_child);

        auto res = mounted_child->update_with(*child);
        if (res.needs_update) {
          mounted_child->mark_dirty();
          pending_redraw.push_back(mounted_child);
        }
        if (res.needs_restyle) {
          style_.update_style(component);
        }
      } else {
        mounted_child = ComponentImpl::make(platform_.get_bus(), component, std::move(child), child_state);

        mounted_child->get_layer().render_data = platform_.get_renderer().mount_component(component);

        style_.compile_rules(*mounted_child);
        style_.update_style(*mounted_child);

        mounted_child->mount();
        PROF_MESSAGE("Component Mounted ({})", mounted_child->get_name());

        pending_redraw.push_back(mounted_child);
      }

      return mounted_child;
    }

    void unmount_child(
      ComponentImpl&                                  component,
      const std::list<ComponentImpl::uptr>::iterator& child) {
      ZoneScopedN("Dismount Child");

      platform_.get_renderer().unmount_component(component);
      (*child)->stop_all_animation();

      focus_state_.unfocus();

      PROF_MESSAGE("Component Unmounted ({})", (*child)->get_name());
      component.get_children_impl().erase(child);
    }
  };
} // namespace cydui::detail::ui::services
