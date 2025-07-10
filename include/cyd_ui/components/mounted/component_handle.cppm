// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;

#define STYLE_SETTER_RETURN_TYPE component_handle_t<B>&
#define STYLE_SETTER_RETURN_EXPR this->component->get_style_stack().mark_dirty(); return *this;
#define STYLE_SETTER_REF_CONSTRAINT
#define STYLE_MAP_GETTER this->component->get_style_stack().get_internal_style_override()

#include "../styling/style_setters_detail.h"

export module cydui.components.handle;

import std;
export import reflect;

import fabric.logging;

export import cydui.components.blueprint.concepts;
export import cydui.components.mounted;
export import cydui.animations;


namespace cydui::components {
  export class component_handle_delegate;
  export template <StaticBlueprint B>
  class component_handle_t;

  template <StaticBlueprint B>
  class component_handle_t {
    mounted_component_t* const component;

  public:
    friend class component_handle_delegate;

    explicit component_handle_t(mounted_component_t* c)
        : component(c) {}

    void mark_dirty() {
      component->mark_dirty();
    }

    void force_update() {
      component->force_update();
    }

    B& get_blueprint() {
      return *static_cast<B*>(component->get_blueprint());
    }
    const B& get_blueprint() const {
      return *static_cast<const B*>(component->get_blueprint());
    }

    std::optional<component_handle_t> find_child(const std::string& id) {
      return component->find_child(id).transform([](mounted_component_t* c) {
        return component_handle_t{c};
      });
    }

    std::optional<const component_handle_t> find_child(const std::string& id) const {
      return component->find_child(id).transform([](const mounted_component_t* c) {
        return component_handle_t{c};
      });
    }

    std::list<mounted_component_t*> find_children(const std::string& tag) {
      return component->find_children(tag);
    }

    std::list<const mounted_component_t*> find_children(const std::string& tag) const {
      const auto* c = component;
      return c->find_children(tag);
    }

    std::list<mounted_component_t*> find_descendent(
      const std::string& tag,
      bool               skip_direct_children = false
    ) {
      return component->find_descendents(tag, skip_direct_children);
    }

    std::list<const mounted_component_t*> find_descendent(
      const std::string& tag,
      bool               skip_direct_children = false
    ) const {
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
      return component->get_blueprint()->has_tag(tag);
    }

    bool tag(const std::string& tag) const {
      bool has_tag = component->get_blueprint()->has_tag(tag);
      component->get_blueprint()->tag(tag);
      return not has_tag;
    }

    bool untag(const std::string& tag) const {
      bool has_tag = component->get_blueprint()->has_tag(tag);
      component->get_blueprint()->untag(tag);
      return has_tag;
    }

    animation_handle animate(const animation& animation) {
      return cydui::animate(*component, animation);
    }

    mounted_component_t* get() const {
      return component;
    }

#include "../styling/style_setters.inc"
  };

  class component_handle_delegate {
  public:
    template <typename B>
    static mounted_component_t& get_ref(component_handle_t<B>& handle) {
      return *handle.component;
    }
  };

} // namespace cydui::components
