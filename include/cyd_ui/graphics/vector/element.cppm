// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include <cairomm-1.16/cairomm/cairomm.h>
#include <tracy/Tracy.hpp>

export module cydui.graphics.vector.element;

import std;

import fabric.memory.multidim_data;
import fabric.type_aliases;

export import cydui.dimensions;
export import cydui.graphics.vector.attributes;

export namespace vg {
  struct element_t {
    int origin_x = 0;
    int origin_y = 0;
    void _internal_set_origin(int x, int y) {
      origin_x = x;
      origin_y = y;
    }

    virtual ~element_t() = default;

    virtual void apply_to(pixelmap_editor_t &editor) const = 0;

    struct footprint {
      // int x, y, w, h;
      cydui::dimensions::screen_measure x;
      cydui::dimensions::screen_measure y;
      cydui::dimensions::screen_measure w;
      cydui::dimensions::screen_measure h;
    };
    virtual footprint get_footprint() const = 0;
  };
}
