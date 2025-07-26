// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include <cairomm-1.16/cairomm/cairomm.h>
#include <tracy/Tracy.hpp>

export module cydui.graphics.vector.elements.line;

import std;

export import cydui.dimensions;
export import cydui.graphics.vector.element;

export namespace vg {
  struct line:
    element_t,
    attrs_core<line>,
    attrs_stroke<line>,
    attr_x1<line>,
    attr_y1<line>,
    attr_x2<line>,
    attr_y2<line> {
    line() {
      // per element Defaults
      this->stroke_width(1);
      //TracyAllocN(this, sizeof(decltype(*this)), "fragment_elements");
    }
    ~line() override {
      //TracyFreeN(this, "fragment_elements");
    }

    void apply_to(cydui::graphics::pixelmap_editor_t &editor) const override {
      apply_stroke(editor);

      set_source_to_stroke(editor);

      if ((_stroke_width % 2) != 0) {
        editor->move_to(origin_x + _x1 + 0.5, origin_y + _y1 + 0.5);
        editor->line_to(origin_x + _x2 + 0.5, origin_y + _y2 + 0.5);
      } else {
        editor->move_to(origin_x + _x1, origin_y + _y1);
        editor->line_to(origin_x + _x2, origin_y + _y2);
      }

      editor->stroke();
    }

    footprint get_footprint() const override {
      return {
        std::min(_x1, _x2),
        std::min(_y1, _y2),
        std::abs(_x2 - _x1),
        std::abs(_y2 - _y1),
      };
    }
  };
}
