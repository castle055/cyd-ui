/*! \file  CairoSurface.cppm
 *! \brief
 *!
 */

module;
#include <cairomm/surface.h>
#include <cairomm/context.h>
export module cydui.platform.render.cairo.CairoSurface;

import std;
import reflect;

import fabric.logging;
export import cydui.platform.surface;

namespace cydui::platform::render {
  export class CairoSurface {
    Cairo::RefPtr<Cairo::ImageSurface> cairo_surface_;
    Surface                            surface_;

  public:
    CairoSurface(
      int width,
      int height
    )
        : cairo_surface_(
            Cairo::ImageSurface::create(
              Cairo::Surface::Format::ARGB32,
              width,
              height
            )
          ),
          surface_(
            {.type   = SurfaceType::CPU,
             .handle = cairo_surface_->get_data(),
             .size =
               {width,
                height},
             .pitch        = cairo_surface_->get_stride(),
             .pixel_format = PixelFormat::ARGB32}
          ) {}

    const Surface& get_surface() const {
      return surface_;
    }

    const Cairo::RefPtr<Cairo::ImageSurface>& get_cairo_surface() const {
      return cairo_surface_;
    }

    SurfaceType get_surface_type() const {
      return surface_.type;
    }

    PixelFormat get_pixel_format() const {
      return surface_.pixel_format;
    }

    SurfaceSize get_size() const {
      return surface_.size;
    }

    void resize(
      int width,
      int height
    ) {
      if (width == surface_.size.first and height == surface_.size.second) {
        return;
      }
      cairo_surface_  = Cairo::ImageSurface::create(Cairo::Surface::Format::ARGB32, width, height);
      surface_.handle = cairo_surface_->get_data();
      surface_.size   = {width, height};
      surface_.pitch  = cairo_surface_->get_stride();
    }

    void clear() {
      auto ctx = Cairo::Context::create(cairo_surface_);
      ctx->set_source_rgba(0,0,0,0);
      ctx->set_operator(Cairo::Context::Operator::SOURCE);
      ctx->paint();
    }
  };
} // namespace cydui::platform::render
