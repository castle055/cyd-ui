// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

export module cydui.components.base:state;

import std;

import fabric.logging;
import fabric.async;

export import cydui.events;

export import :holder;
// export import :event_handler;

export class with_context;

export template<typename ContextType>
struct provide_context;

export using children_list = std::vector<cydui::components::component_holder_t>;

namespace cydui::components {
  export class component_state_delegate_t;

  struct component_state_t {
    virtual ~component_state_t() = default;

    component_state_t() = default;
    explicit component_state_t(void* props) { }


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
      _dirty = true;
    }

    template<fabric::async::EventType EV>
    void emit(EV ev) {
      if (nullptr == this->window) return;
      this->window->emit<EV>(ev);
    }

    /*!
     * @brief Emits a RedrawEvent for this component.
     *
     * @warning This is only meant to be called from custom event handlers.
     * Event handlers for events declared in `cydui_events.h`
     * automatically trigger a redraw if `state->mark_dirty()` gets called.
     * Use that instead.
     */
    void force_redraw() {
      mark_dirty();
      emit<RedrawEvent>({.component = this});
    }

    std::string component_name() const {
      return component_name_;
    }

    component_state_t* parent() {
      return parent_;
    }

    void add_children_state(const std::string& id, const component_state_ref &child) {
      children_states[id] = child;
      child->parent_ = this;
      child->window = window;
    }

    bool is_animated() const {
      return animated;
    }

    bool is_text_input() const {
      return is_text_input_;
    }

  private:
    friend class component_base_t;
    friend class component_state_delegate_t;


  protected:
    void set_component_name(const std::string& name) {
      component_name_ = name;
    }

  public:
    std::string component_name_;

    component_state_t* parent_ = nullptr;
    std::shared_ptr<fabric::async::async_bus_t> window = nullptr;

    std::optional<std::shared_ptr<component_base_t>> component_instance = std::nullopt;
    std::unordered_map<std::string, component_state_ref> children_states { };

    bool _dirty = false;
    bool focused = false;
    bool hovering = false;

  private:
    bool animated = false;
    bool is_text_input_ = false;
  };

  class component_state_delegate_t {
  public:
    static void set_animated(component_state_t* it, bool animated) {
      it->animated = animated;
    }

    static void set_is_text_input(component_state_t* it, bool is_text_input) {
      it->is_text_input_ = is_text_input;
    }
  };
}