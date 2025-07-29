// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include <tracy/Tracy.hpp>

export module cydui.elements.Rectangle;

import std;

export import cydui.dimensions;
export import cydui.elements.attributes;
export import cydui.elements.footprint;

export namespace cydui::elements {
  struct Rectangle: attributes::attrs_core<Rectangle>,
                    attributes::attrs_fill<Rectangle>,
                    attributes::attrs_border<Rectangle>,
                    attributes::attr_x<Rectangle>,
                    attributes::attr_y<Rectangle>,
                    attributes::attr_w<Rectangle>,
                    attributes::attr_h<Rectangle>,
                    attributes::attr_rotate<Rectangle> {
    Rectangle() {
      // TracyAllocN(this, sizeof(decltype(*this)), "fragment_elements");
    }
    ~Rectangle() {
      // TracyFreeN(this, "fragment_elements");
    }

    Footprint get_footprint() const {
      return {_x, _y, _w, _h};
    }
  };
} // namespace cydui::elements
