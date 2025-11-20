// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

export module cydui.core.aspects.controller;
export import cydui.core.aspects.controller.base;

import std;
import reflect;

import fabric.logging;
import fabric.async;
import fabric.exception;

import cydui.core.state;
export import cydui.core.aspects.reference;
export import cydui.core.aspects.contexts.store;
export import cydui.core.aspects.contexts.events;


namespace cydui {
  export template <typename T>
  concept HasController =                                                  //
    ComponentBlueprint<T>                                                  //
    and requires { typename detail::event_handler_type<T>::controller_t; } //
    and std::
      derived_from<typename detail::event_handler_type<T>::controller_t, ControllerBase<detail::event_handler_type<T>>>;

  namespace detail {
    export template <HasController T>
    using controller_type = event_handler_type<T>::controller_t;

    std::shared_ptr<void> get_event_handler(Component* component);
  } // namespace detail

  export template <HasController C>
  class controller: public reference<C> {
  public:
    using controller_type = detail::controller_type<C>;
    struct opts_t {};

  private:
    opts_t                         opts {};
    std::optional<controller_type> controller_ {std::nullopt};

    void on_reference_set(const TypedComponent<C>& target_component) override {
      auto* evh = static_cast<detail::event_handler_type<C>*>(detail::get_event_handler(target_component.get()).get());
      controller_.emplace(ControllerBase<detail::event_handler_type<C>> {*evh});
    }

  public:
    controller() = default;

    explicit controller(const opts_t& opts)
        : opts(opts) {}


    controller_type* operator->() {
      if (not controller_.has_value()) {
        throw fabric::exception {std::format(
          "Null controller reference: {}::controller<{}>",
          this->get_component()->get_blueprint().get_name(),
          refl::type_name<C>)};
      }

      return &controller_.value();
    }
  };
} // namespace cydui
