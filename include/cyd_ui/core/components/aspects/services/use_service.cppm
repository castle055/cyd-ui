// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

export module cydui.core.aspects.use_service;

import std;
import reflect;

import fabric.logging;
import fabric.async;
import fabric.exception;

import cydui.core.state;
export import cydui.core.aspects;
export import cydui.core.aspects.contexts.store;
export import cydui.core.aspects.contexts.events;


namespace cydui {
  export template <typename T>
  class use_service final: private ComponentAspect {
  public:
    struct opts_t {};

  private:
    opts_t             opts {};
    std::shared_ptr<T> service_ {nullptr};

    void on_mount() override {
      auto service_opt = get_service_context().template find<T>();
      if (service_opt.has_value()) {
        service_ = std::move(service_opt.value());
      } else {
        throw fabric::exception(
          std::format(
            "Could not find service '{}' required by component '{}'",
            refl::type_name<T>,
            get_component()->get_blueprint().get_name()));
      }
    }

  public:
    use_service() = default;

    explicit use_service(const opts_t& opts)
        : opts(opts) {}

    T* operator->() {
      return service_.get();
    }
  };
} // namespace cydui
