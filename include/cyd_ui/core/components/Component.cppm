// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;

#define STYLE_SETTER_RETURN_TYPE Component&
#define STYLE_SETTER_RETURN_EXPR                                                                                       \
  this->get_style_stack().mark_dirty();                                                                                \
  return *this;
#define STYLE_SETTER_REF_CONSTRAINT
#define STYLE_MAP_GETTER this->get_style_stack().get_internal_style_override()

#include "../style/include/style_setters_detail.h"

export module cydui.core.Component:interface;

import std;
import reflect;

export import cydui.core.blueprint.base;
export import cydui.core.blueprint.concepts;
export import cydui.core.aspects.contexts.store;

export import cydui.geometry;
export import cydui.layer;
export import cydui.styling.style_stack;
export import cydui.animations.Animation;
export import cydui.animations.AnimationHandle;


export namespace cydui {
  template <detail::StaticBlueprint ComponentBlueprint>
  class TypedComponent;

  class Component {
  public:
    using uptr = std::unique_ptr<Component>;

    virtual ~Component() = default;

    virtual std::list<Component*>       get_children()       = 0;
    virtual std::list<const Component*> get_children() const = 0;

    virtual bool is_root() const = 0;

    virtual Component*       get_parent()       = 0;
    virtual const Component* get_parent() const = 0;

    virtual layout::component_geometry& get_geometry() const = 0;

    virtual const style::style_base_t& get_style() const = 0;

    virtual void mark_dirty() = 0;

    virtual Blueprint&       get_blueprint()       = 0;
    virtual const Blueprint& get_blueprint() const = 0;

    virtual std::optional<Component*> find_child(const std::string& id) = 0;

    virtual std::optional<const Component*> find_child(const std::string& id) const = 0;

    virtual std::list<Component*> find_children(const std::string& tag) = 0;

    virtual std::list<const Component*> find_children(const std::string& tag) const = 0;

    virtual std::list<Component*> find_descendents(
      const std::string& tag,
      bool               skip_direct_children = false) = 0;

    virtual std::list<const Component*> find_descendents(
      const std::string& tag,
      bool               skip_direct_children = false) const = 0;

    virtual bool is_focused() const = 0;
    virtual bool is_hovered() const = 0;

    virtual animations::AnimationHandle animate(const animations::Animation& anim) = 0;

    template <detail::StaticBlueprint ComponentBlueprint>
    TypedComponent<ComponentBlueprint> as();

    bool has_tag(const std::string& tag) const {
      return get_blueprint().has_tag(tag);
    }

    bool tag(const detail::tag_type& tag) {
      bool has_tag = get_blueprint().has_tag(tag);
      get_blueprint().tag(tag);
      return not has_tag;
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

    bool untag(const detail::tag_type& tag) {
      bool has_tag = get_blueprint().has_tag(tag);
      get_blueprint().untag(tag);
      return has_tag;
    }

#include "../style/include/style_setters.inc"


    template <detail::StaticBlueprint ComponentBlueprint>
    friend class TypedComponent;

  private:
    virtual style::style_stack& get_style_stack() = 0;
  };
} // namespace cydui
