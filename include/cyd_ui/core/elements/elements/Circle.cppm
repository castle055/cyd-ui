// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include <tracy/Tracy.hpp>

export module cydui.elements.Circle;

  import std;

export import cydui.dimensions;
export import cydui.elements.attributes;
export import cydui.elements.footprint;

export namespace cydui::elements {
  struct Circle: attributes::attrs_core<Circle>,
                 attributes::attrs_fill<Circle>,
                 attributes::attrs_stroke<Circle>,
                 attributes::attr_cx<Circle>,
                 attributes::attr_cy<Circle>,
                 attributes::attr_r<Circle> {
    Circle() {
      // TracyAllocN(this, sizeof(decltype(*this)), "fragment_elements");
    }
    ~Circle() {
      // TracyFreeN(this, "fragment_elements");
    }

    Footprint get_footprint() const {
      return {_cx - _r, _cy - _r, 2 * _r, 2 * _r};
    }
  };
} // namespace cydui::elements
