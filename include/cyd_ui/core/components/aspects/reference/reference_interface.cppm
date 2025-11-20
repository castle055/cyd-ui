// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

export module cydui.core.aspects.reference.iface;

import std;
import reflect;

namespace cydui::detail {
  export struct reference_interface {
    virtual ~reference_interface()                  = default;
    virtual void set_reference(void* component_ptr) = 0;
  };

  export using reference_set = std::set<reference_interface*>;
} // namespace cydui::detail
