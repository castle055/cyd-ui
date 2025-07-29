// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include <tracy/Tracy.hpp>

export module cydui.elements.Ellipse;

import std;

export import cydui.dimensions;
export import cydui.elements.attributes;
export import cydui.elements.footprint;

export namespace cydui::elements {
  struct Ellipse: attributes::attrs_core<Ellipse>,
                  attributes::attrs_fill<Ellipse>,
                  attributes::attrs_stroke<Ellipse>,
                  attributes::attr_cx<Ellipse>,
                  attributes::attr_cy<Ellipse>,
                  attributes::attr_rx<Ellipse>,
                  attributes::attr_ry<Ellipse> {
    Ellipse() {
      // TracyAllocN(this, sizeof(decltype(*this)), "fragment_elements");
    }
    ~Ellipse() {
      // TracyFreeN(this, "fragment_elements");
    }

    Footprint get_footprint() const {
      return {_cx - _rx, _cy - _ry, 2 * _rx, 2 * _ry};
    }
  };
} // namespace cydui::elements
