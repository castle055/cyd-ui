// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

export module cydui.core.aspects;

import std;
import reflect;

import fabric.logging;
import fabric.async;
import fabric.services;

export import cydui.core.Component;
export import cydui.core.aspects.contexts.store;


namespace cydui {
  export class ComponentAspect {
    fabric::async::async_bus_t*       bus_ {nullptr};
    fabric::services::ServiceContext* service_context_ {nullptr};
    Component*                        component_ {nullptr};
    detail::context_store_t*          context_store_ {nullptr};

  protected:
    virtual void on_mount() {}
    virtual void on_update() {}

    fabric::async::async_bus_t& get_bus() {
      return *bus_;
    }

    fabric::services::ServiceContext& get_service_context() {
      return *service_context_;
    }

    Component* get_component() {
      return component_;
    }

    detail::context_store_t& get_context_store() {
      return *context_store_;
    }

    void update_component() {
      component_->mark_dirty();
      bus_->emit(RedrawEvent {});
    }

  public:
    ComponentAspect()          = default;
    virtual ~ComponentAspect() = default;

    void mount(
      fabric::async::async_bus_t&       bus,
      fabric::services::ServiceContext& service_context,
      Component*                        component,
      detail::context_store_t&          context_store) {
      bus_             = &bus;
      service_context_ = &service_context;
      component_       = component;
      context_store_   = &context_store;
      on_mount();
    }

    void update() {
      on_update();
    }
  };

  export template <typename Aspect>
  concept IsComponentAspect = std::is_base_of_v<ComponentAspect, Aspect>;

  namespace detail {
    export template <IsComponentAspect Aspect>
    inline constexpr ComponentAspect& get_aspect_base(Aspect& val) {
      return *reinterpret_cast<ComponentAspect*>(&val);
    }
  } // namespace detail
} // namespace cydui
