// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include <cairomm-1.16/cairomm/cairomm.h>
#include <tracy/Tracy.hpp>

export module cydui.graphics.vector.elements.pixelmap;

import std;

export import cydui.dimensions;
export import cydui.graphics.vector.element;

export namespace vg {
  struct pixelmap:
    element_t,
    attrs_core<pixelmap>,
    attr_x<pixelmap>,
    attr_y<pixelmap>,
    attr_rotate<pixelmap>,
    attr_pivot_x<pixelmap>,
    attr_pivot_y<pixelmap>,
    attr_scale_x<pixelmap>,
    attr_scale_y<pixelmap>,
    attr_w<pixelmap>,
    attr_h<pixelmap> {
    const pixelmap_t &_pxm;
    explicit pixelmap(const pixelmap_t &pxm): _pxm(pxm) {
      //TracyAllocN(this, sizeof(decltype(*this)), "fragment_elements");
    }
    ~pixelmap() override {
      //TracyFreeN(this, "fragment_elements");
    }

    void apply_to(pixelmap_editor_t &editor) const override {
      // TODO - cairo_format_stride_for_width() should be called before allocating the image buffer and thus use its
      // output to determine the 'width' of the buffer.
      Cairo::RefPtr<Cairo::Surface> surf = Cairo::ImageSurface::create(
        (unsigned char*) this->_pxm.data,
        Cairo::Surface::Format::ARGB32,
        (int) this->_pxm.width(),
        (int) this->_pxm.height(),
        (int) this->_pxm.width() * 4
      );
      editor->save();
      editor->translate(origin_x + _x + _pivot_x, origin_y + _y + _pivot_y);
      editor->rotate(_rotate * std::numbers::pi / 180.0);
      editor->scale(_scale_x, _scale_y);
      editor->set_source(surf, -_pivot_x, -_pivot_y);
      editor->paint_with_alpha(_opacity);
      editor->restore();

      surf->finish();
    }

    footprint get_footprint() const override {
      return {};
    }
  };
}
