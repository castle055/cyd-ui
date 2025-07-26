// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include <cairomm-1.16/cairomm/cairomm.h>
#include <tracy/Tracy.hpp>

export module cydui.graphics.vector.elements.circle;

import std;

export import cydui.dimensions;
export import cydui.graphics.vector.element;

export namespace vg {
  struct circle:
    element_t,
    attrs_core<circle>,
    attrs_fill<circle>,
    attrs_stroke<circle>,
    attr_cx<circle>,
    attr_cy<circle>,
    attr_r<circle> {
    circle() {
      //TracyAllocN(this, sizeof(decltype(*this)), "fragment_elements");
    }
    ~circle() override {
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
      editor->begin_new_path();
      editor->translate(origin_x + _cx + odd_offset, origin_y + _cy + odd_offset);
      editor->arc(0.0, 0.0, _r, 0, 2 * std::numbers::pi);
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
