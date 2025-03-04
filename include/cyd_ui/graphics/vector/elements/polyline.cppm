// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include <cairomm-1.16/cairomm/cairomm.h>
#include <tracy/Tracy.hpp>

export module cydui.graphics.vector.elements.polyline;

import std;

export import cydui.dimensions;
export import cydui.graphics.vector.element;

export namespace vg {
  struct polyline:
    element_t,
    attrs_core<polyline>,
    attrs_fill<polyline>,
    attrs_stroke<polyline>,
    attr_points<polyline> {
    polyline() {
      //TracyAllocN(this, sizeof(decltype(*this)), "fragment_elements");
    }
    ~polyline() override {
      //TracyFreeN(this, "fragment_elements");
    }
    void apply_to(pixelmap_editor_t &editor) const override {
      apply_stroke(editor);
      apply_fill(editor);

      bool first = true;
      double odd_offset = 0.0;
      if ((_stroke_width % 2) != 0) {
        odd_offset = 0.5;
      }
      for (const auto &p: _points) {
        if (first) {
          editor->move_to(origin_x + p[0] + odd_offset, origin_y + p[1] + odd_offset);
          first = false;
        } else {
          editor->line_to(origin_x + p[0] + odd_offset, origin_y + p[1] + odd_offset);
        }
      }

      set_source_to_fill(editor);
      editor->fill_preserve();

      set_source_to_stroke(editor);
      editor->stroke();
    }

    footprint get_footprint() const override {
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
        if (x < min_x) min_x = x;
        if (x > max_x) max_x = x;
        if (y < min_y) min_y = y;
        if (y > max_y) max_y = y;
      }
      return {
        min_x,
        min_y,
        max_x - min_x,
        max_y - min_y,
      };
    }
  };
}
