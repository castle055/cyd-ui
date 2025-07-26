// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include <cairomm-1.16/cairomm/cairomm.h>
#include <tracy/Tracy.hpp>

export module cydui.graphics.vector.elements.arc;

import std;

export import cydui.dimensions;
export import cydui.graphics.vector.element;

export namespace vg {
  struct arc:
    element_t,
    attrs_core<arc>,
    attrs_fill<arc>,
    attrs_stroke<arc>,
    attr_cx<arc>,
    attr_cy<arc>,
    attr_r<arc>,
    attr_a1<arc>,
    attr_a2<arc> {
    arc() {
      //TracyAllocN(this, sizeof(decltype(*this)), "fragment_elements");
    }
    ~arc() override {
      //TracyFreeN(this, "fragment_elements");
    }
    void apply_to(cydui::graphics::pixelmap_editor_t &editor) const override {
      editor->save();
      apply_stroke(editor);
      apply_fill(editor);

      double odd_offset = 0.0;
      if ((_stroke_width % 2) != 0) {
        odd_offset = 0.5;
      }
      editor->translate(origin_x + _cx + odd_offset, origin_y + _cy + odd_offset);
      //editor->scale(_rx, _ry);
      editor->arc(0.0, 0.0, _r, _a1 * 2 * std::numbers::pi / 360.0, _a2 * 2 * std::numbers::pi / 360.0);

      set_source_to_fill(editor);
      editor->fill_preserve();

      set_source_to_stroke(editor);
      editor->stroke();

      editor->restore();
    }

    footprint get_footprint() const override {
      return {_cx - _r, _cy - _r, 2 * _r, 2 * _r};
    }
  };
}
