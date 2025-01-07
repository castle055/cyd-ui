// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

export module cydui.components:base;

import std;

import fabric.logging;
import fabric.memory.lazy_alloc;
import fabric.async;

import cydui.graphics;

export import :attributes;
export import :with_specialization;

export import :state;

namespace cyd::ui::components {
  struct event_handler_t;
  export struct component_base_t {
    using sptr = std::shared_ptr<component_base_t>;

    virtual ~component_base_t() = default; //{
    //  ! All of this is done in the `component_t` class destructor
    //  clear_subscribed_listeners();
    //  if (state.has_value()) {
    //    state.value()->component_instance = std::nullopt;
    //  }
    //}
    virtual void clear_children() = 0;
    virtual attrs_component<>* attrs() = 0;
    virtual void* get_props() = 0;
    virtual std::string name() = 0;

    virtual std::shared_ptr<event_handler_t> event_handler() = 0;
    virtual void dispatch_key_press(const KeyEvent& ev) = 0;
    virtual void dispatch_key_release(const KeyEvent& ev) = 0;
    virtual void dispatch_button_press(const Button& ev, dimension_t::value_type x, dimension_t::value_type y) = 0;
    virtual void dispatch_button_release(const Button& ev, dimension_t::value_type x, dimension_t::value_type y) = 0;
    virtual void dispatch_scroll(dimension_t::value_type dx, dimension_t::value_type dy) = 0;
    virtual void dispatch_mouse_enter(dimension_t::value_type x, dimension_t::value_type y) = 0;
    virtual void dispatch_mouse_exit(dimension_t::value_type x, dimension_t::value_type y) = 0;
    virtual void dispatch_mouse_motion(dimension_t::value_type x, dimension_t::value_type y) = 0;

    virtual void redraw() = 0;
    virtual void
    get_fragment(
      std::unique_ptr<cyd::ui::compositing::compositing_node_t> &compositing_node
    ) = 0;

    virtual std::shared_ptr<component_state_t> create_state_instance() = 0;

    virtual component_base_t* find_by_coords(dimension_t::value_type x, dimension_t::value_type y) = 0;

    virtual attrs_dimensions<>& get_dimensional_relations() = 0;
    virtual std::shared_ptr<dimension_ctx_t> get_dimensional_context() = 0;

    internal_relations_t& get_internal_relations() {
      return internal_relations;
    }

  public:
    component_state_ref state() const {
      if (state_.has_value()) {
        return state_.value().lock();
      } else {
        return {nullptr};
      }
    }

    virtual void configure_event_handler() = 0;

  protected:
    void set_state(const component_state_ref& state) {
      state_ = state;
    }

    /**
     * \brief Update props and attributes with other component
     * \param other
     * \return [bool] true if we need to redraw
     */
    virtual bool update_with(std::shared_ptr<component_base_t> other) = 0;

    template<ComponentEventHandlerConcept EVH, typename T>
    friend struct component_t;

    internal_relations_t internal_relations{};

  public:
    std::optional<component_base_t*> parent = std::nullopt;
    std::list<std::shared_ptr<component_base_t>> children{};
  private:
    std::optional<std::weak_ptr<component_state_t>> state_ = std::nullopt;
  };
}