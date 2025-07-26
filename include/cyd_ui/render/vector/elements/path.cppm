// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include <cairomm-1.16/cairomm/cairomm.h>
#include <tracy/Tracy.hpp>

export module cydui.graphics.vector.elements.path;

import std;

export import cydui.dimensions;
export import cydui.graphics.vector.element;

export namespace vg {
  struct path:
    element_t,
    attrs_core<path>,
    attrs_fill<path>,
    attrs_stroke<path>,
    attr_path_str<path> {
    path() {
      //TracyAllocN(this, sizeof(decltype(*this)), "fragment_elements");
    }
    ~path() override {
      //TracyFreeN(this, "fragment_elements");
    }
    void apply_to(cydui::graphics::pixelmap_editor_t &editor) const override {
      apply_stroke(editor);
      apply_fill(editor);
    }

    footprint get_footprint() const override {
      return {};
    }
  };
}
