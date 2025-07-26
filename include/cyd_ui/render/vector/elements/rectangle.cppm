// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include <cairomm-1.16/cairomm/cairomm.h>
#include <tracy/Tracy.hpp>

export module cydui.graphics.vector.elements.rectangle;

import std;

export import cydui.dimensions;
export import cydui.graphics.vector.element;

export namespace vg {
  struct rectangle:
    element_t,
    attrs_core<rectangle>,
    attrs_fill<rectangle>,
    attrs_stroke<rectangle>,
    attr_x<rectangle>,
    attr_y<rectangle>,
    attr_w<rectangle>,
    attr_h<rectangle>,
    attr_r<rectangle> {
    rectangle() {
      //TracyAllocN(this, sizeof(decltype(*this)), "fragment_elements");
    }
    ~rectangle() override {
      //TracyFreeN(this, "fragment_elements");
    }
    void apply_to(cydui::graphics::pixelmap_editor_t &editor) const override {
      apply_stroke(editor);
      apply_fill(editor);

      double odd_offset = 0.0;
      if ((_stroke_width % 2) != 0) {
        odd_offset = 0.5;
      }
      auto x = origin_x + _x + odd_offset;
      auto y = origin_y + _y + odd_offset;

      if (_r == 0) {
        editor->rectangle(x, y, _w, _h);
      } else {
        auto r = _r;
        r = std::min(_w / 2, r);
        r = std::min(_h / 2, r);
        editor->move_to(x + r, y);
        editor->arc(x + _w - r, y + r, r, 3 * M_PI / 2, 0);
        editor->arc(x + _w - r, y + _h - r, r, 0, M_PI / 2);
        editor->arc(x + r, y + _h - r, r, M_PI / 2, M_PI);
        editor->arc(x + r, y + r, r, M_PI, 3 * M_PI / 2);
      }

      set_source_to_fill(editor);
      editor->fill_preserve();

      set_source_to_stroke(editor);
      editor->stroke();
    }

    footprint get_footprint() const override {
      return {_x, _y, _w, _h};
    }
  };
}
