// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;

#define STYLE_SETTER_RETURN_TYPE TypedComponent&
#define STYLE_SETTER_RETURN_EXPR                                                                                       \
  this->component->get_style_stack().mark_dirty();                                                                     \
  return *this;
#define STYLE_SETTER_REF_CONSTRAINT
#define STYLE_MAP_GETTER this->component->get_style_stack().get_internal_style_override()

#include <cyd_fabric_modules/headers/macros/async_events.h>

#include "../style/include/style_setters_detail.h"

export module cydui.core.Component;
export import :interface;

import std;
import reflect;

export import cydui.core.blueprint.base;
export import cydui.core.blueprint.concepts;
export import cydui.core.aspects.contexts.store;

export import cydui.geometry;
export import cydui.layer;
export import cydui.styling.style_stack;


export namespace cydui {
  EVENT(RequestComponentFocus) {
    Component* component;
  };

  template <detail::StaticBlueprint ComponentBlueprint>
  class TypedComponent {
    Component* const component;

  public:
    friend class component_handle_delegate;

    explicit TypedComponent(Component* c)
        : component(c) {}

    void mark_dirty() {
      component->mark_dirty();
    }

    ComponentBlueprint& get_blueprint() {
      return *static_cast<ComponentBlueprint*>(&component->get_blueprint());
    }
    const ComponentBlueprint& get_blueprint() const {
      return *static_cast<const ComponentBlueprint*>(&component->get_blueprint());
    }

    ComponentBlueprint* operator->() {
      return static_cast<ComponentBlueprint*>(&component->get_blueprint());
    }
    const ComponentBlueprint* operator->() const {
      return static_cast<const ComponentBlueprint*>(&component->get_blueprint());
    }

    std::list<Component*> get_children() {
      return component->get_children();
    }

    std::list<const Component*> get_children() const {
      return std::as_const(*component).get_children();
    }

    layout::component_geometry& get_geometry() {
      return component->get_geometry();
    }

    const layout::component_geometry& get_geometry() const {
      return std::as_const(*component).get_geometry();
    }

    std::optional<Component*> find_child(const std::string& id) {
      return component->find_child(id);
    }

    std::optional<const Component*> find_child(const std::string& id) const {
      return component->find_child(id);
    }

    std::list<Component*> find_children(const std::string& tag) {
      return component->find_children(tag);
    }

    std::list<const Component*> find_children(const std::string& tag) const {
      const auto* c = component;
      return c->find_children(tag);
    }

    std::list<Component*> find_descendent(
      const std::string& tag,
      bool               skip_direct_children = false) {
      return component->find_descendents(tag, skip_direct_children);
    }

    std::list<const Component*> find_descendent(
      const std::string& tag,
      bool               skip_direct_children = false) const {
      const auto* c = component;
      return c->find_descendents(tag, skip_direct_children);
    }

    bool is_focused() const {
      return component->is_focused();
    }

    bool is_hovered() const {
      return component->is_hovered();
    }

    bool has_tag(const std::string& tag) const {
      return component->has_tag(tag);
    }

    bool tag(const std::string& tag) const {
      return component->tag(tag);
    }

    void tag(
      const detail::tag_type& tag,
      bool                    tagged) {
      if (tagged) {
        this->tag(tag);
      } else {
        untag(tag);
      }
    }

    bool untag(const std::string& tag) const {
      return component->untag(tag);
    }

    animations::AnimationHandle animate(const animations::Animation& animation) {
      return component->animate(animation);
    }

    Component* get() const {
      return component;
    }

#include "../style/include/style_setters.inc"
  };


  template <detail::StaticBlueprint ComponentBlueprint>
  TypedComponent<ComponentBlueprint> Component::as() {
    return TypedComponent<ComponentBlueprint>(this);
  }
} // namespace cydui
