// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include <tracy/Tracy.hpp>

export module cydui.elements.Polygon;

import std;

export import cydui.dimensions;
export import cydui.elements.attributes;
export import cydui.elements.footprint;


export namespace cydui::elements {
  struct Polygon: attributes::attrs_core<Polygon>,
                  attributes::attrs_fill<Polygon>,
                  attributes::attrs_stroke<Polygon>,
                  attributes::attr_points<Polygon> {
    Polygon() {
      // TracyAllocN(this, sizeof(decltype(*this)), "fragment_elements");
    }
    ~Polygon() {
      // TracyFreeN(this, "fragment_elements");
    }

    Footprint get_footprint() const {
      if (_points.empty()) {
        return {0, 0, 0, 0};
      }

      int min_x = _points[0][0];
      int min_y = _points[0][1];
      int max_x = _points[0][0];
      int max_y = _points[0][1];
      int x, y;
      for (std::size_t i = 1; i < _points.size(); ++i) {
        x = _points[i][0];
        y = _points[i][1];
        if (x < min_x)
          min_x = x;
        if (x > max_x)
          max_x = x;
        if (y < min_y)
          min_y = y;
        if (y > max_y)
          max_y = y;
      }
      return {
        min_x,
        min_y,
        max_x - min_x,
        max_y - min_y,
      };
    }
  };
} // namespace cydui::elements
