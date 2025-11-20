// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;

#define STYLE_SETTER_RETURN_TYPE ComponentImpl&
#define STYLE_SETTER_RETURN_EXPR                                                                                       \
  this->get_style_stack().mark_dirty();                                                                                \
  return *this;
#define STYLE_SETTER_REF_CONSTRAINT
#define STYLE_MAP_GETTER this->get_style_stack().get_internal_style_override()

#include "../../../include/cyd_ui/core/style/include/style_setters_detail.h"

export module cydui.core.Component.impl;
export import :parent_reference;

import std;
import reflect;
export import fabric.services;

export import cydui.core.Component;
export import cydui.core.blueprint.base;
export import cydui.core.aspects.contexts.store;

export import cydui.geometry;
export import cydui.layer;
export import cydui.styling.style_stack;
export import cydui.animations.Animation;
export import cydui.animations.AnimationHandle;


export namespace cydui::detail {
  class ComponentImpl final: public Component {
  public:
    using uptr = std::unique_ptr<ComponentImpl>;

  private:
    fabric::services::ServiceContext& internal_services_;
    fabric::services::ServiceContext& ui_services_;

    [[refl::ignore]]
    Blueprint::uptr     blueprint_;
    ComponentIdentifier id_;

    parent_reference_t parent_;
    [[refl::ignore]]
    std::list<uptr> children_;

    ComponentState::sptr state_;
    [[refl::ignore]]
    style::style_stack style_stack_;
    [[refl::ignore]]
    std::unique_ptr<layout::component_geometry> geometry_;
    Layer                                       render_layer_ {};
    context_store_t                             context_store_;

    [[refl::ignore]]
    std::unique_ptr<event_dispatcher_base_t> event_dispatcher_;

    bool __dirty__ = false;

    bool hovering_      = false;
    bool focused_       = false;
    bool is_text_input_ = false;

    std::list<animations::AnimationHandle> animations_ {};

    ComponentImpl(
      fabric::async::async_bus_t&       bus,
      fabric::services::ServiceContext& internal_services,
      fabric::services::ServiceContext& ui_services,
      parent_reference_t                parent,
      Blueprint::uptr                   blueprint,
      const ComponentState::sptr&       state);

  public:
    static ComponentImpl* make(
      fabric::async::async_bus_t& bus,
      ComponentImpl&              parent,
      Blueprint::uptr             blueprint,
      const ComponentState::sptr& state);

    static uptr make_root(
      fabric::async::async_bus_t&       bus,
      fabric::services::ServiceContext& internal_services,
      fabric::services::ServiceContext& ui_services,
      Blueprint::uptr                   blueprint);

    static ComponentImpl* from_interface(Component* component) {
      return dynamic_cast<ComponentImpl*>(component);
    }

    ~ComponentImpl() override;

    /*!
     * @brief Marks this component state as needing to be redrawn.
     *
     * Events declared in `cydui_events.h` are dispatched automatically
     * by the layout, which then checks for any `dirty` components that need
     * redrawing.
     *
     * It is due diligence of the component developer to appropriately mark
     * the component as dirty when appropriate. As a baseline, any mutation
     * to the state of a component should mark it dirty if said mutation
     * affects how the component, or its children, are rendered.
     */
    void mark_dirty() override;

    bool is_dirty() const;

    void clear_dirty_flag();

    bool is_hovered() const override;

    void set_hovered(bool hovered);

    bool is_focused() const override;

    void focus();

    void unfocus();

    bool is_text_input() const;

    bool is_animated() const;

    animations::AnimationHandle start_animation(animations::AnimationHandle handle);

    void stop_all_animation();

    void mount();

    update_result update_with(const Blueprint& other);

    std::optional<Component*> find_child(const std::string& id) override;

    std::optional<const Component*> find_child(const std::string& id) const override;

    std::list<Component*> find_children(const std::string& tag) override;

    std::list<const Component*> find_children(const std::string& tag) const override;

    std::list<Component*> find_descendents(
      const std::string& tag,
      bool               skip_direct_children = false) override;

    std::list<const Component*> find_descendents(
      const std::string& tag,
      bool               skip_direct_children = false) const override;

    std::optional<ComponentImpl*> find_child_impl(const std::string& id);

    std::optional<const ComponentImpl*> find_child_impl(const std::string& id) const;

    std::list<ComponentImpl*> find_children_impl(const std::string& tag);

    std::list<const ComponentImpl*> find_children_impl(const std::string& tag) const;

    std::list<ComponentImpl*> find_descendents_impl(
      const std::string& tag,
      bool               skip_direct_children = false);

    std::list<const ComponentImpl*> find_descendents_impl(
      const std::string& tag,
      bool               skip_direct_children = false) const;

    bool is_root() const override;

    const ComponentIdentifier& get_id() const;

    const std::string& get_name() const;

    ComponentState::sptr get_state() const;

    Blueprint& get_blueprint() override;

    const Blueprint& get_blueprint() const override;

    std::list<Component*>       get_children() override;
    std::list<const Component*> get_children() const override;

    std::list<uptr>& get_children_impl();

    const std::list<uptr>& get_children_impl() const;

    Component*       get_parent() override;
    const Component* get_parent() const override;

    const parent_reference_t& get_parent_impl() const;

    event_dispatcher_base_t& get_event_dispatcher() const;

    layout::component_geometry& get_geometry();

    layout::component_geometry& get_geometry() const override;

    Layer& get_layer();

    const Layer& get_layer() const;

    const style::style_base_t& get_style() const override;

    style::style_stack& get_style_stack() override;

    animations::AnimationHandle animate(const animations::Animation& anim) override;

#include "../../../include/cyd_ui/core/style/include/style_setters.inc"
  };
} // namespace cydui::detail
