// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include <tracy/Tracy.hpp>

export module cydui.elements.Arc;

import std;

export import cydui.dimensions;
export import cydui.elements.attributes;
export import cydui.elements.footprint;

export namespace cydui::elements {
  struct Arc: attributes::attrs_core<Arc>,
              attributes::attrs_fill<Arc>,
              attributes::attrs_stroke<Arc>,
              attributes::attr_include_center_point<Arc>,
              attributes::attr_cx<Arc>,
              attributes::attr_cy<Arc>,
              attributes::attr_r<Arc>,
              attributes::attr_a1<Arc>,
              attributes::attr_a2<Arc> {
    Arc() {
      // TracyAllocN(this, sizeof(decltype(*this)), "fragment_elements");
    }
    ~Arc() {
      // TracyFreeN(this, "fragment_elements");
    }
    Footprint get_footprint() const {
      return {_cx - _r, _cy - _r, 2 * _r, 2 * _r};
    }
  };
} // namespace cydui::elements
