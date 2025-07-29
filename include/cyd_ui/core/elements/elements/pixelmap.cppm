// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include <tracy/Tracy.hpp>

export module cydui.elements.Pixelmap;

import std;

export import cydui.dimensions;
export import cydui.elements.attributes;
export import cydui.elements.footprint;

export namespace cydui::elements {
  struct Pixelmap: attributes::attrs_core<Pixelmap>,
                   attributes::attr_x<Pixelmap>,
                   attributes::attr_y<Pixelmap>,
                   attributes::attr_rotate<Pixelmap>,
                   attributes::attr_pivot_x<Pixelmap>,
                   attributes::attr_pivot_y<Pixelmap>,
                   attributes::attr_w<Pixelmap>,
                   attributes::attr_h<Pixelmap> {
    cydui::graphics::pixelmap_t _pxm;
    explicit Pixelmap(const cydui::graphics::pixelmap_t& pxm)
        : _pxm(pxm) {
      // TracyAllocN(this, sizeof(decltype(*this)), "fragment_elements");
    }
    ~Pixelmap() {
      // TracyFreeN(this, "fragment_elements");
    }

    Footprint get_footprint() const {
      return {};
    }
  };
} // namespace cydui::elements
