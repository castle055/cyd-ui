// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include <cairomm-1.16/cairomm/cairomm.h>

export module cydui.graphics:pixelmap_editor;

export import cydui.graphics.types;

// CAIRO_HAS_PDF_SURFACE

export {
class pixelmap_editor_t {
public:
  pixelmap_t &img;
  
  Cairo::RefPtr<Cairo::Surface> surface;
  Cairo::RefPtr<Cairo::Context> ctx;
public:
  explicit pixelmap_editor_t(pixelmap_t &img): img(img) {
    // TODO - cairo_format_stride_for_width() should be called before allocating the image buffer and thus use its
    // output to determine the 'width' of the buffer.
    // Cairo::PdfSurface::create("",1,1);
    surface = Cairo::ImageSurface::create(
      (unsigned char*) this->img.data,
      Cairo::Surface::Format::ARGB32,
      (int) this->img.width(), (int) this->img.height(),
      (int) this->img.width() * 4
      //cairo_format_stride_for_width(CAIRO_FORMAT_ARGB32, (int) this->img.width())
    );
    ctx = Cairo::Context::create(surface);
  }
  
  explicit pixelmap_editor_t(pixelmap_t* img): pixelmap_editor_t(*img) {
  }

public:
  Cairo::RefPtr<Cairo::Context> operator->() {
    return ctx;
  }
  
  void clear() {
    memset(img.data, 0, img.width() * img.height() * sizeof(pixel_t));
  }
};

}
