// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;

#define STYLE_SETTER_RETURN_TYPE mounted_component_t&
#define STYLE_SETTER_RETURN_EXPR this->get_style_stack().mark_dirty(); return *this;
#define STYLE_SETTER_REF_CONSTRAINT
#define STYLE_MAP_GETTER this->get_style_stack().get_internal_style_override()

#include "../styling/style_setters_detail.h"

export module cydui.components.mounted;
export import :parent_reference;

import std;

import fabric.logging;
import fabric.async;
import fabric.templates.functor_arguments;

export import cydui.components.blueprint.base;
export import cydui.components.contexts.store;

export import cydui.graphics.compositing_node;

export import cydui.geometry;
export import cydui.styling.style_stack;


export namespace cydui::components {
  class mounted_component_t {
  public:
    using uptr = std::unique_ptr<mounted_component_t>;

  private:
    [[refl::ignore]]
    blueprint_base_t::uptr blueprint_;
    identifier_t           id_;

    parent_reference_t               parent_;
    [[refl::ignore]] std::list<uptr> children_;
    backends::frame_base::sptr       window_;

    component_state_t::sptr                                        state_;
    [[refl::ignore]] style_stack                                   style_stack_;
    [[refl::ignore]] std::unique_ptr<geometry::component_geometry> geometry_;
    compositing::compositing_node_t::sptr                          compositing_node_;
    context_store_t                                                context_store_;

    [[refl::ignore]]
    std::unique_ptr<event_dispatcher_base_t> event_dispatcher_;

    bool __dirty__ = false;

    bool hovering_      = false;
    bool focused_       = false;
    bool is_text_input_ = false;

    unsigned int animation_count_ = 0;

  private:
    explicit mounted_component_t(
      const backends::frame_base::sptr& window,
      parent_reference_t                parent,
      blueprint_base_t::uptr            blueprint,
      const component_state_t::sptr&    state
    )
        : blueprint_(std::move(blueprint)),
          id_(blueprint_->get_id()),
          parent_(parent),
          children_{},
          window_(window),
          state_(state == nullptr ? blueprint_->make_state_object() : state),
          style_stack_(
            blueprint_->get_style_type_info(),
            blueprint_->make_style_object()
          ),
          geometry_(geometry::component_geometry::make(get_name())),
          compositing_node_(
            std::make_shared<compositing::compositing_node_t>(
              parent_.is_valid() ? parent_->compositing_node_ : nullptr
            )
          ),
          context_store_({}),
          event_dispatcher_(blueprint_->make_event_dispatcher(this)) {
    }

  public:
    static mounted_component_t* make(
      mounted_component_t&    parent,
      blueprint_base_t::uptr  blueprint,
      component_state_t::sptr state
    ) {
      auto ptr = std::unique_ptr<mounted_component_t>(new mounted_component_t{
        parent.window_, parent_reference_t{&parent}, std::move(blueprint), state
      });

      auto* ret = ptr.get();
      parent.children_.emplace_back(std::move(ptr));

      return ret;
    }

    static uptr make(
      const backends::frame_base::sptr& window,
      blueprint_base_t::uptr            blueprint
    ) {
      return std::unique_ptr<mounted_component_t>(
        new mounted_component_t{window, parent_reference_t{nullptr}, std::move(blueprint), nullptr}
      );
    }

    ~mounted_component_t() {
      event_dispatcher_->dispatch_dismount();
    }

  public:
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
    void mark_dirty() {
      __dirty__ = true;
    }

    /*!
     * @brief Emits a RedrawEvent for this component.
     *
     * @warning This is only meant to be called from custom event handlers.
     * Event handlers for events declared in `cydui_events.h`
     * automatically trigger a redraw if `state->mark_dirty()` gets called.
     * Use that instead.
     */
    void force_update() {
      mark_dirty();
      window_->emit<RedrawEvent>({.component = this});
    }

    bool is_dirty() const {
      return __dirty__;
    }

    void clear_dirty_flag() {
      __dirty__ = false;
    }

    bool is_hovered() const {
      return hovering_;
    }

    void set_hovered(bool hovered) {
      hovering_ = hovered;
    }

    bool is_focused() const {
      return focused_;
    }

    void focus() {
      if (not focused_) {
        focused_ = true;
        event_dispatcher_->dispatch_focus_changed();
      }
    }

    void unfocus() {
      if (focused_) {
        focused_ = false;
        event_dispatcher_->dispatch_focus_changed();
      }
    }

    bool is_text_input() const {
      return is_text_input_;
    }

    bool is_animated() const {
      return animation_count_ > 0;
    }

    void start_animation() {
      animation_count_++;
    }

    void stop_animation() {
      animation_count_--;
    }

    bool update_with(const blueprint_base_t& other) {
      return blueprint_->update_with(other);
    }

    std::optional<mounted_component_t*> find_child(const std::string& id) {
      for (auto& child: children_) {
        if (child->get_id().str() == id) {
          return child.get();
        }
      }
      return std::nullopt;
    }

    std::optional<const mounted_component_t*> find_child(const std::string& id) const {
      for (const auto& child: children_) {
        if (child->get_id().str() == id) {
          return child.get();
        }
      }
      return std::nullopt;
    }

    std::list<mounted_component_t*> find_children(const std::string& tag) {
      std::list<mounted_component_t*> result{};
      for (auto& child: children_) {
        if (child->get_blueprint()->has_tag(tag)) {
          result.emplace_back(child.get());
        }
      }
      return result;
    }

    std::list<const mounted_component_t*> find_children(const std::string& tag) const {
      std::list<const mounted_component_t*> result{};
      for (const auto& child: children_) {
        if (child->get_id().str() == tag) {
          result.emplace_back(child.get());
        }
      }
      return result;
    }

    std::list<mounted_component_t*> find_descendents(
      const std::string& tag,
      bool               skip_direct_children = false
    ) {
      std::list<mounted_component_t*> result{};
      if (not skip_direct_children) {
        for (auto& child: children_) {
          if (child->get_blueprint()->has_tag(tag)) {
            result.emplace_back(child.get());
          }
        }
      }
      for (auto& child: children_) {
        auto descendants = child->find_descendents(tag, false);
        for (auto& d: descendants) {
          result.emplace_back(d);
        }
      }
      return result;
    }

    std::list<const mounted_component_t*> find_descendents(
      const std::string& tag,
      bool               skip_direct_children = false
    ) const {
      std::list<const mounted_component_t*> result{};
      if (not skip_direct_children) {
        for (const auto& child: children_) {
          if (child->get_blueprint()->has_tag(tag)) {
            result.emplace_back(child.get());
          }
        }
      }
      for (const auto& child: children_) {
        auto descendants = child->find_descendents(tag, false);
        for (const auto& d: descendants) {
          result.emplace_back(d);
        }
      }
      return result;
    }

    template <typename ContextType>
    void add_context(const std::shared_ptr<ContextType>& ptr) {
      context_store_.add_context<ContextType>(ptr);
    }

    template <typename ContextType>
    std::optional<std::shared_ptr<ContextType>> find_context() {
      if (not context_store_.empty()) {
        auto context = context_store_.find_context<ContextType>();
        if (context.has_value()) {
          return context;
        }
      }

      if (not is_root()) {
        return parent_->find_context<ContextType>();
      }

      return std::nullopt;
    }

  public:
    bool is_root() const {
      return not parent_.is_valid();
    }

    const identifier_t& get_id() const {
      return id_;
    }

    const std::string& get_name() const {
      return blueprint_->get_name();
    }

    component_state_t::sptr get_state() const {
      return state_;
    }

    blueprint_base_t* get_blueprint() {
      return blueprint_.get();
    }

    const blueprint_base_t* get_blueprint() const {
      return blueprint_.get();
    }

    auto& get_children() {
      return children_;
    }

    const auto& get_children() const {
      return children_;
    }

    const parent_reference_t& get_parent() const {
      return parent_;
    }

    event_dispatcher_base_t& get_event_dispatcher() const {
      return *event_dispatcher_;
    }

    geometry::component_geometry& get_geometry() {
      return *geometry_;
    }

    const geometry::component_geometry& get_geometry() const {
      return *geometry_;
    }

    compositing::compositing_node_t& get_compositing_node() {
      return *compositing_node_;
    }

    const compositing::compositing_node_t& get_compositing_node() const {
      return *compositing_node_;
    }

    const style_base_t& get_style() const {
      return style_stack_.get_style_object();
    }

    style_stack& get_style_stack() {
      return style_stack_;
    }

    const backends::frame_base::sptr& get_window() const {
      return window_;
    }

#include "../styling/style_setters.inc"
  };
} // namespace cydui::components
