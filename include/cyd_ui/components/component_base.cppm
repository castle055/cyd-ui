// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include "cyd_fabric_modules/headers/macros/async_events.h"

export module cydui.components:base;

import std;

import fabric.logging;
import fabric.memory.lazy_alloc;
import fabric.async;
import fabric.templates.functor_arguments;

import cydui.graphics;

export import :attributes;
export import :with_specialization;

export import :state;

export template<typename ContextType>
struct ContextUpdate {
  constexpr static const char* type = refl::type_name<ContextUpdate<ContextType>>.data();
  ContextType* ptr = nullptr;
};

export class with_context {
public:
  with_context(std::initializer_list<cyd::ui::components::component_holder_t> &&components) {
    std::size_t i {0};
    for (auto &&holder: components) {
      for (auto &[id, c]: holder.get_components()) {
        components_.emplace_back(std::format(":{}{}", i, id), c);
      }
      ++i;
    }
  }

  cyd::ui::components::component_holder_t build() const {
    return cyd::ui::components::component_holder_t {components_};
  }

private:
  // std::vector<cyd::ui::components::component_holder_t> components_;
  std::vector<std::pair<std::string, std::shared_ptr<cyd::ui::components::component_base_t>>> components_ { };
};


namespace cyd::ui::components {
  struct event_handler_t;

  export class context_store_t {
  public:
    context_store_t() = default;

    template<typename T>
    void add_context(const std::shared_ptr<T> &ptr) {
      static constexpr refl::type_id_t type_id = refl::type_id<T>;
      std::shared_ptr<void> ptr_               = std::static_pointer_cast<void>(ptr);

      context_map_[type_id] = ptr_;
    }

    template<typename T>
    std::optional<T*> find_context() {
      static constexpr refl::type_id_t type_id = refl::type_id<T>;
      if (context_map_.contains(type_id)) {
        return static_cast<T*>(context_map_[type_id].get());
      }
      return std::nullopt;
    }

    bool empty() const {
      return context_map_.empty();
    }
  private:
    std::unordered_map<refl::type_id_t, std::shared_ptr<void>> context_map_ { };
  };

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

    virtual std::shared_ptr<component_state_t> create_state_instance() = 0;

    virtual component_base_t* find_by_coords(dimension_t::value_type x, dimension_t::value_type y) = 0;

    virtual attrs_dimensions<>& get_dimensional_relations() = 0;
    virtual std::shared_ptr<dimension_ctx_t> get_dimensional_context() = 0;

    internal_relations_t& get_internal_relations() {
      return internal_relations;
    }

  protected:
    virtual void get_fragment(cyd::ui::compositing::compositing_node_t &compositing_node) = 0;

  public:
    void render() {
      for (auto& child : children) {
        child->render();
      }

      if (graphics_dirty_) {
        graphics_dirty_ = false;
        this->get_fragment(compositing_node_);
        compositing_node_.render();
      }
    }

    compositing::compositing_node_t* compose(graphics::window_t* render_target) {
      compositing_node_.clear_composite_texture(render_target);
      compositing_node_.compose_own(render_target);
      for (auto& child : children) {
        compositing_node_.compose(render_target, child->compose(render_target));
      }
      return &compositing_node_;
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

  public:
    template<typename ContextType>
    void add_context(provide_context<ContextType>& ptr) {
      context_store_.add_context<ContextType>(ptr);
    }

  private:
    friend class ::with_context;

    template<typename ContextType>
    friend struct ::provide_context;

    template<typename ContextType>
    void add_context(const std::shared_ptr<ContextType> &ptr) {
      context_store_.add_context<ContextType>(ptr);
    }

    auto& get_context_store() {
      return context_store_;
    }

    template<typename ContextType>
    std::optional<ContextType*> find_context() {
      if (not context_store_.empty()) {
        auto context = context_store_.find_context<ContextType>();
        if (context.has_value()) {
          return context;
        }
      }

      if (parent.has_value()) {
        return parent.value()->find_context<ContextType>();
      } else {
        return std::nullopt;
      }
    }
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

    context_store_t context_store_{};

    compositing::compositing_node_t compositing_node_ { };
    bool graphics_dirty_ = true;
  };
}

export template<typename ContextType>
struct use_context {
  using context_type = ContextType;

  template<cyd::ui::components::ComponentEventHandlerConcept EVH, typename T>
  friend struct cyd::ui::components::component_t;

  use_context() = default;

  use_context(const use_context& other): ctx(other.ctx), state(other.state) {
    if (other.owns_context) {
      this->owns_context = true;
      other.owns_context = false;
    }
  }

  use_context& operator=(const use_context& other) {
    stop_listening();
    this->ctx = other.ctx;
    this->state = other.state;
    if (other.owns_context) {
      this->owns_context = true;
      other.owns_context = false;
    }
  }

  use_context(use_context&& other) noexcept: ctx(other.ctx), state(other.state) {
    other.stop_listening();
    if (other.owns_context) {
      this->owns_context = true;

      other.owns_context = false;
      other.ctx          = nullptr;
    }
  }

  use_context& operator=(use_context&& other) {
    other.stop_listening();
    stop_listening();
    this->ctx = other.ctx;
    this->state = other.state;
    if (other.owns_context) {
      this->owns_context = true;
      other.owns_context = false;
    }
  }

  ~use_context() {
    stop_listening();
    if (owns_context) {
      delete ctx;
    }
  }

  context_type* operator->() {
    return ctx;
  }

  context_type &operator*() {
    return *ctx;
  }

  void notify() {
    state->emit<ContextUpdate<context_type>>({ctx});
  }

  private:
    void start_listening() {
      stop_listening();

      listener = state->window->on_event([&](ContextUpdate<context_type> ev) {
        if (ev.ptr == ctx) {
          state->force_redraw();
        }
      });
    }
    void stop_listening() {
      if (listener.has_value()) {
        listener.value().remove();
        listener = std::nullopt;
      }
    }
private:
  // This object will own the context if it couldn't be found and thus a default one was created
  bool owns_context                                  = false;
  context_type* ctx                                  = nullptr;
  cyd::ui::components::component_state_ref state     = nullptr;

  std::optional<fabric::async::listener<ContextUpdate<context_type>>> listener {std::nullopt};
};

export template<typename ContextType>
struct provide_context {
  using context_type = ContextType;

  template<cyd::ui::components::ComponentEventHandlerConcept EVH, typename T>
  friend struct cyd::ui::components::component_t;

  provide_context() = default;

  provide_context(const provide_context &other) = default;

  provide_context &operator=(const provide_context &other) = default;

  provide_context(provide_context &&other) noexcept = default;

  provide_context &operator=(provide_context &&other) = default;

  context_type* operator->() {
    return context_.get();
  }

  context_type &operator*() {
    return *context_;
  }

  void notify() {
    state->emit<ContextUpdate<context_type>>({context_.get()});
  }

  cyd::ui::components::component_holder_t operator >(with_context &&components) {
    auto holder = components.build();

    for (auto &[_, c]: holder.get_components()) {
      if (not c->get_context_store().find_context<ContextType>().has_value()) {
        c->add_context(context_);
      }
    }

    return holder;
  }

  operator std::shared_ptr<context_type>() {
    return context_;
  }

private:
  // This object will own the context if it couldn't be found and thus a default one was created
  cyd::ui::components::component_state_ref state = nullptr;
  std::shared_ptr<context_type> context_         = std::make_shared<context_type>();
};

