// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include <tracy/Tracy.hpp>

export module cydui.elements.Line;

import std;

export import cydui.dimensions;
export import cydui.elements.attributes;
export import cydui.elements.footprint;

export namespace cydui::elements {
  struct Line: attributes::attrs_core<Line>,
               attributes::attrs_stroke<Line>,
               attributes::attr_x1<Line>,
               attributes::attr_y1<Line>,
               attributes::attr_x2<Line>,
               attributes::attr_y2<Line> {
    Line() {
      // per element Defaults
      this->stroke_width(1);
      // TracyAllocN(this, sizeof(decltype(*this)), "fragment_elements");
    }

    ~Line() {
      // TracyFreeN(this, "fragment_elements");
    }

    Footprint get_footprint() const {
      return {
        std::min(_x1, _x2),
        std::min(_y1, _y2),
        std::abs(_x2 - _x1),
        std::abs(_y2 - _y1),
      };
    }
  };
} // namespace cydui::elements
