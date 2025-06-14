// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

export module cydui.components.mounted:parent_reference;

import std;
export import reflect;

import fabric.logging;


namespace cydui::components {
  export class mounted_component_t;

  export class parent_reference_t {
    mounted_component_t* parent_;

  public:
    explicit parent_reference_t(mounted_component_t* parent)
        : parent_(parent) {}

    mounted_component_t* operator->() const {
      return parent_;
    }

    mounted_component_t& operator*() const {
      return *parent_;
    }

    bool is_valid() const {
      return parent_ != nullptr;
    }
  };
} // namespace cydui::components
