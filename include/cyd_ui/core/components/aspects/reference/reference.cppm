// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

export module cydui.core.aspects.reference;
export import cydui.core.aspects.reference.iface;

import std;
import reflect;

import fabric.logging;
import fabric.async;
import fabric.exception;

import cydui.core.state;
export import cydui.core.aspects;


namespace cydui {
  export template <ComponentBlueprint C>
  class reference: protected ComponentAspect, public detail::reference_interface {
  public:
    using blueprint_type = C;
    struct opts_t {};

  private:
    opts_t opts {};

  protected:
    std::optional<TypedComponent<blueprint_type>> target_component_ {std::nullopt};

    virtual void on_reference_set(const TypedComponent<blueprint_type>& target_component) {}

  private:
    void set_reference(void* ptr) final {
      target_component_.emplace(TypedComponent<blueprint_type> {static_cast<Component*>(ptr)});
      on_reference_set(target_component_.value());
    }

  public:
    reference()           = default;
    ~reference() override = default;

    explicit reference(const opts_t& opts)
        : opts(opts) {}

    bool is_bound() const noexcept {
      return target_component_.has_value();
    }

    TypedComponent<blueprint_type>* operator->() {
      TypedComponent<blueprint_type> asdf {};
      return &target_component_;
    }
  };
} // namespace cydui
