// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

export module cydui.core.Component.impl:parent_reference;

import std;
export import reflect;
import fabric.logging;


namespace cydui::detail {
  export class ComponentImpl;

  export class parent_reference_t {
    ComponentImpl* parent_;

  public:
    explicit parent_reference_t(ComponentImpl* parent)
        : parent_(parent) {}

    ComponentImpl* operator->() const {
      return parent_;
    }

    ComponentImpl& operator*() const {
      return *parent_;
    }

    bool is_valid() const {
      return parent_ != nullptr;
    }
  };
} // namespace cydui::components
