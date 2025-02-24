// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include "cyd_fabric_modules/headers/macros/async_events.h"

export module cydui.components.base;

import std;

import fabric.logging;
import fabric.async;
import fabric.templates.functor_arguments;

import cydui.graphics;
export import cydui.styling;
export import cydui.components.base.style;

export import :event_dispatcher;
export import :attributes;
export import :contexts;
export import :state;

namespace cyd::ui::components {
  export class component_base_t {
  public:
    using sptr = std::shared_ptr<component_base_t>;
    using wptr = std::weak_ptr<component_base_t>;

  public:
    virtual ~component_base_t() = default; //{
    //  ! All of this is done in the `component_t` class destructor
    //  clear_subscribed_listeners();
    //  if (state.has_value()) {
    //    state.value()->component_instance = std::nullopt;
    //  }
    //}
    virtual void* get_props() = 0;

    virtual component_base_t* find_by_coords(dimension_t::value_type x, dimension_t::value_type y) = 0;

    virtual attrs_dimensions<>& get_dimensional_relations() = 0;
    virtual std::shared_ptr<dimension_ctx_t> get_dimensional_context() = 0;

    virtual const refl::type_info& get_style_type_info() const = 0;

    virtual attrs_component<>* attrs() = 0;
    virtual std::string name() const = 0;

  private:
    virtual std::shared_ptr<component_state_t> create_state_instance() = 0;

    /**
     * \brief Update props and attributes with other component
     * \param other
     * \return [bool] true if we need to redraw
     */
    virtual bool update_with(std::shared_ptr<component_base_t> other) = 0;

    virtual void mount() = 0;
    virtual void dismount() = 0;

  public:
    component_state_ref state() const {
      if (state_.has_value()) {
        return state_.value().lock();
      } else {
        return {nullptr};
      }
    }

    template <typename T>
    auto& get_data() {
      using type = std::remove_const_t<std::remove_reference_t<T>>;
      static auto tid = refl::type_id<type>;

      if (not data_map_.contains(tid)) {
        data_map_[tid] = refl::any::make<type>();
      }

      return data_map_.at(tid).template as<type>();
    }

    template <typename T>
    bool remove_data() {
      using type = std::remove_const_t<std::remove_reference_t<T>>;
      static auto tid = refl::type_id<type>;

      if (data_map_.contains(tid)) {
        data_map_.erase(tid);
        return true;
      }
      return false;
    }

    template<typename ContextType>
    void add_context(provide_context<ContextType>& ptr) {
      context_store_.add_context<ContextType>(ptr);
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
      }

      return std::nullopt;
    }

    internal_relations_t& get_internal_relations() {
      return internal_relations;
    }

    event_dispatcher_base_t* get_event_dispatcher() {
      return event_dispatcher.has_value()? event_dispatcher.value().get(): nullptr;
    }

    style_base_t& get_style() {
      return style_data->as_base();
    }

    style_data_base_t& get_style_data() {
      return *(style_data.get());
    }


    void clear_children() {
      children.clear();
    }

    void set_id(const std::string& id) {
      id_ = id;
    }

    std::string get_id() const {
      return id_;
    }

    std::optional<sptr> find_child(const std::string& id) {
      for (const auto & child : children) {
        if (child->get_id() == id) {
          return child;
        }
        auto descendant = child->find_child(id);
        if (descendant.has_value()) {
          return descendant;
        }
      }
      return std::nullopt;
    }

    std::list<sptr> find_children(const std::string& id) {
      std::list<sptr> result{};
      for (const auto & child : children) {
        if (child->get_id() == id) {
          result.push_back(child);
        }
        auto descendants = child->find_children(id);
        for (const auto & d : descendants) {
          result.push_back(d);
        }
      }
      return result;
    }

  protected:
    void set_state(const component_state_ref& state) {
      state_ = state;
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

  public:
    friend struct component_actor_t;

    std::optional<component_base_t*> parent = std::nullopt;
    std::list<std::shared_ptr<component_base_t>> children{};

  protected:
    internal_relations_t internal_relations{};
    std::optional<std::shared_ptr<event_dispatcher_base_t>> event_dispatcher{std::nullopt};
    std::shared_ptr<style_data_base_t> style_data{};

  private:
    std::optional<std::weak_ptr<component_state_t>> state_ = std::nullopt;
    context_store_t context_store_{};
    std::unordered_map<refl::type_id_t, refl::any> data_map_ { };

    std::string id_{};
  };

  struct component_actor_t {
    static void set_component_state(component_base_t* component, component_state_ref state) {
      component->set_state(state);
    }
    static std::shared_ptr<component_state_t> create_state_instance(component_base_t* component) {
      return component->create_state_instance();
    }

    static bool update_component_with(component_base_t* component, std::shared_ptr<component_base_t> other) {
      return component->update_with(other);
    }

    static void mount_component(component_base_t* component) {
      component->mount();
    }
    static void dismount_component(component_base_t* component) {
      component->dismount();
    }
  };
}

template<typename ContextType>
cyd::ui::components::component_holder_t provide_context<ContextType>::operator >(with_context &&components) {
  auto holder = components.build();

  for (auto &[_, c]: holder.get_components()) {
    if (not c->get_context_store().find_context<ContextType>().has_value()) {
      c->add_context(context_);
    }
  }

  return holder;
}
