// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include <cairomm-1.16/cairomm/cairomm.h>
#include <tracy/Tracy.hpp>

export module cydui.graphics.vector.elements.ellipse;

import std;

export import cydui.dimensions;
export import cydui.graphics.vector.element;

export namespace vg {
  struct ellipse:
    element_t,
    attrs_core<ellipse>,
    attrs_fill<ellipse>,
    attrs_stroke<ellipse>,
    attr_cx<ellipse>,
    attr_cy<ellipse>,
    attr_rx<ellipse>,
    attr_ry<ellipse> {
    ellipse() {
      //TracyAllocN(this, sizeof(decltype(*this)), "fragment_elements");
    }
    ~ellipse() override {
      //TracyFreeN(this, "fragment_elements");
    }
    void apply_to(cydui::graphics::pixelmap_editor_t &editor) const override {
      apply_stroke(editor);
      apply_fill(editor);

      editor->save();
      double odd_offset = 0.0;
      if ((_stroke_width % 2) != 0) {
        odd_offset = 0.5;
      }
      editor->translate(origin_x + _cx + odd_offset, origin_y + _cy + odd_offset);
      editor->scale(_rx, _ry);
      editor->arc(0.0, 0.0, 1.0, 0.0, 2 * std::numbers::pi);

      set_source_to_fill(editor);
      editor->fill_preserve();

      set_source_to_stroke(editor);
      editor->stroke();

      editor->restore();
    }

    footprint get_footprint() const override {
      return {_cx - _rx, _cy - _ry, 2 * _rx, 2 * _ry};
    }
  };
}
