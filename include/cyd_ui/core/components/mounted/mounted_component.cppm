// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;

#define STYLE_SETTER_RETURN_TYPE mounted_component_t&
#define STYLE_SETTER_RETURN_EXPR                                                                   \
  this->get_style_stack().mark_dirty();                                                            \
  return *this;
#define STYLE_SETTER_REF_CONSTRAINT
#define STYLE_MAP_GETTER this->get_style_stack().get_internal_style_override()

#include "../../style/include/style_setters_detail.h"

export module cydui.core.mounted;
export import :parent_reference;

import std;
import reflect;

export import cydui.core.blueprint.base;
export import cydui.core.contexts.store;

export import cydui.graphics.compositing_node;

export import cydui.geometry;
export import cydui.styling.style_stack;


export namespace cydui::core {
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

    component_state_t::sptr                                      state_;
    [[refl::ignore]] style::style_stack                          style_stack_;
    [[refl::ignore]] std::unique_ptr<layout::component_geometry> geometry_;
    compositing::compositing_node_t::sptr                        compositing_node_;
    context_store_t                                              context_store_;

    [[refl::ignore]]
    std::unique_ptr<event_dispatcher_base_t> event_dispatcher_;

    bool __dirty__ = false;

    bool hovering_      = false;
    bool focused_       = false;
    bool is_text_input_ = false;

    unsigned int animation_count_ = 0;

    explicit mounted_component_t(
      const backends::frame_base::sptr& window,
      parent_reference_t                parent,
      blueprint_base_t::uptr            blueprint,
      const component_state_t::sptr&    state
    );

  public:
    static mounted_component_t* make(
      mounted_component_t&    parent,
      blueprint_base_t::uptr  blueprint,
      component_state_t::sptr state
    );

    static uptr make(
      const backends::frame_base::sptr& window,
      blueprint_base_t::uptr            blueprint
    );

    ~mounted_component_t();

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
    void mark_dirty();

    /*!
     * @brief Emits a RedrawEvent for this component.
     *
     * @warning This is only meant to be called from custom event handlers.
     * Event handlers for events declared in `cydui_events.h`
     * automatically trigger a redraw if `state->mark_dirty()` gets called.
     * Use that instead.
     */
    void force_update();

    bool is_dirty() const;

    void clear_dirty_flag();

    bool is_hovered() const;

    void set_hovered(bool hovered);

    bool is_focused() const;

    void focus();

    void unfocus();

    bool is_text_input() const;

    bool is_animated() const;

    void start_animation();

    void stop_animation();

    bool update_with(const blueprint_base_t& other);

    std::optional<mounted_component_t*> find_child(const std::string& id);

    std::optional<const mounted_component_t*> find_child(const std::string& id) const;

    std::list<mounted_component_t*> find_children(const std::string& tag);

    std::list<const mounted_component_t*> find_children(const std::string& tag) const;

    std::list<mounted_component_t*> find_descendents(
      const std::string& tag,
      bool               skip_direct_children = false
    );

    std::list<const mounted_component_t*> find_descendents(
      const std::string& tag,
      bool               skip_direct_children = false
    ) const;

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

    bool is_root() const;

    const identifier_t& get_id() const;

    const std::string& get_name() const;

    component_state_t::sptr get_state() const;

    blueprint_base_t* get_blueprint();

    const blueprint_base_t* get_blueprint() const;

    std::list<uptr>& get_children();

    const std::list<uptr>& get_children() const;

    const parent_reference_t& get_parent() const;

    event_dispatcher_base_t& get_event_dispatcher() const;

    layout::component_geometry& get_geometry();

    const layout::component_geometry& get_geometry() const;

    compositing::compositing_node_t& get_compositing_node();

    const compositing::compositing_node_t& get_compositing_node() const;

    const style::style_base_t& get_style() const;

    style::style_stack& get_style_stack();

    const backends::frame_base::sptr& get_window() const;

#include "../../style/include/style_setters.inc"
  };
} // namespace cydui::core
