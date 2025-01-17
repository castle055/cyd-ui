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
  int width, height;
  pixel_t *img = nullptr;
  
  Cairo::RefPtr<Cairo::Surface> surface;
  Cairo::RefPtr<Cairo::Context> ctx;
public:
  explicit pixelmap_editor_t(pixelmap_t &img): width(img.width()), height(img.height()), img(img.data) {
    // TODO - cairo_format_stride_for_width() should be called before allocating the image buffer and thus use its
    // output to determine the 'width' of the buffer.
    // Cairo::PdfSurface::create("",1,1);
    surface = Cairo::ImageSurface::create(
      (unsigned char*) this->img,
      Cairo::Surface::Format::ARGB32,
      width, height,
      width * 4
      //cairo_format_stride_for_width(CAIRO_FORMAT_ARGB32, (int) this->img.width())
    );
    ctx = Cairo::Context::create(surface);
    // ctx->set_antialias(Cairo::Antialias::ANTIALIAS_SUBPIXEL);
  }
  
  explicit pixelmap_editor_t(pixelmap_t* img): pixelmap_editor_t(*img) {
  }

  explicit pixelmap_editor_t(int width_, int height_, pixel_t* img_, int stride): width(width_), height(height_), img(img_) {
    // TODO - cairo_format_stride_for_width() should be called before allocating the image buffer and thus use its
    // output to determine the 'width' of the buffer.
    // Cairo::PdfSurface::create("",1,1);
    surface = Cairo::ImageSurface::create(
      (unsigned char*) img,
      Cairo::Surface::Format::ARGB32,
      width, height,
      stride
      //cairo_format_stride_for_width(CAIRO_FORMAT_ARGB32, (int) this->img.width())
    );
    ctx = Cairo::Context::create(surface);
    // ctx->set_antialias(Cairo::Antialias::ANTIALIAS_SUBPIXEL);
  }

public:
  Cairo::RefPtr<Cairo::Context> operator->() {
    return ctx;
  }
  
  void clear() {
    memset(img, 0, width * height * sizeof(pixel_t));
  }
};

}
