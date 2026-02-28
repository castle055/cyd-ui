/*! \file  ElementPainters.cppm
 *! \brief
 *!
 */

module;
#include <pangomm.h>

#include "../../../../debug/profiling/macros.h"
#include "cairomm/context.h"
#define PROF_CURRENT_MODULE cydui::platform::render::cairo::ElementPainters

#define ELEMENT_PAINTER(ELEMENT)                                                                                       \
  template <typename T>                                                                                                \
  struct element_painter;                                                                                              \
  template <>                                                                                                          \
  struct element_painter<cydui::elements::ELEMENT> {                                                                   \
    static void paint(                                                                                                 \
      const Cairo::RefPtr<Cairo::Context>& ctx,                                                                        \
      const cydui::elements::ELEMENT&      element);                                                                        \
  };                                                                                                                   \
  void element_painter<cydui::elements::ELEMENT>::paint(                                                               \
    const Cairo::RefPtr<Cairo::Context>& ctx, const cydui::elements::ELEMENT& element)

#define SET_BORDER_LINE(EDGE)                                                                                          \
  apply_paint_to_source(ctx, element._border_##EDGE, element._border_opacity_##EDGE);                                  \
  ctx->set_line_width(element._border_width_##EDGE);                                                                   \
  ctx->set_line_cap(map_line_cap(element._border_linecap_##EDGE));                                                     \
  ctx->set_line_join(map_line_join(element._border_linejoin_##EDGE));                                                  \
  ctx->set_dash(element._border_dasharray_##EDGE, element._border_dashoffset_##EDGE);                                  \
  ctx->set_miter_limit(element._border_miterlimit_##EDGE)

export module cydui.platform.render.cairo.ElementPainters;

import std;
import reflect;

import fabric.logging;
export import cydui.layer;
export import cydui.platform.render.cairo;
export import cydui.platform.render.cairo.CairoSurface;
export import cydui.elements;
import cydui.elements.attributes;
import cydui.debug.profiling;

using Context = Cairo::RefPtr<Cairo::Context>;

Cairo::Context::LineCap map_line_cap(const cydui::elements::attributes::LineCap& value) {
  switch (value) {
    case cydui::elements::attributes::LineCap::BUTT  : return Cairo::Context::LineCap::BUTT;
    case cydui::elements::attributes::LineCap::ROUND : return Cairo::Context::LineCap::ROUND;
    case cydui::elements::attributes::LineCap::SQUARE: return Cairo::Context::LineCap::SQUARE;
    default                                          : return Cairo::Context::LineCap::BUTT;
  }
}

Cairo::Context::LineJoin map_line_join(const cydui::elements::attributes::LineJoin& value) {
  switch (value) {
    case cydui::elements::attributes::LineJoin::MITER: return Cairo::Context::LineJoin::MITER;
    case cydui::elements::attributes::LineJoin::ROUND: return Cairo::Context::LineJoin::ROUND;
    case cydui::elements::attributes::LineJoin::BEVEL: return Cairo::Context::LineJoin::BEVEL;
    default                                          : return Cairo::Context::LineJoin::MITER;
  }
}

Cairo::Context::FillRule map_fill_rule(const cydui::elements::attributes::FillRule& value) {
  switch (value) {
    case cydui::elements::attributes::FillRule::WINDING : return Cairo::Context::FillRule::WINDING;
    case cydui::elements::attributes::FillRule::EVEN_ODD: return Cairo::Context::FillRule::EVEN_ODD;
    default                                             : return Cairo::Context::FillRule::WINDING;
  }
}

Pango::Alignment map_text_align(const cydui::elements::attributes::TextAlign& value) {
  switch (value) {
    case cydui::elements::attributes::TextAlign::LEFT  : return Pango::Alignment::LEFT;
    case cydui::elements::attributes::TextAlign::CENTER: return Pango::Alignment::CENTER;
    case cydui::elements::attributes::TextAlign::RIGHT : return Pango::Alignment::RIGHT;
    default                                            : return Pango::Alignment::LEFT;
  }
}

Pango::Stretch map_text_stretch(const cydui::elements::attributes::TextStretch& value) {
  switch (value) {
    case cydui::elements::attributes::TextStretch::ULTRA_CONDENSED: return Pango::Stretch::ULTRA_CONDENSED;
    case cydui::elements::attributes::TextStretch::EXTRA_CONDENSED: return Pango::Stretch::EXTRA_CONDENSED;
    case cydui::elements::attributes::TextStretch::CONDENSED      : return Pango::Stretch::CONDENSED;
    case cydui::elements::attributes::TextStretch::SEMI_CONDENSED : return Pango::Stretch::SEMI_CONDENSED;
    case cydui::elements::attributes::TextStretch::NORMAL         : return Pango::Stretch::NORMAL;
    case cydui::elements::attributes::TextStretch::SEMI_EXPANDED  : return Pango::Stretch::SEMI_EXPANDED;
    case cydui::elements::attributes::TextStretch::EXPANDED       : return Pango::Stretch::EXPANDED;
    case cydui::elements::attributes::TextStretch::EXTRA_EXPANDED : return Pango::Stretch::EXTRA_EXPANDED;
    case cydui::elements::attributes::TextStretch::ULTRA_EXPANDED : return Pango::Stretch::ULTRA_EXPANDED;
    default                                                       : return Pango::Stretch::NORMAL;
  }
}

Pango::Weight map_text_weight(const cydui::elements::attributes::FontWeight& value) {
  switch (value) {
    case cydui::elements::attributes::FontWeight::THIN  : return Pango::Weight::THIN;
    case cydui::elements::attributes::FontWeight::LIGHT : return Pango::Weight::LIGHT;
    case cydui::elements::attributes::FontWeight::NORMAL: return Pango::Weight::NORMAL;
    case cydui::elements::attributes::FontWeight::MEDIUM: return Pango::Weight::MEDIUM;
    case cydui::elements::attributes::FontWeight::BOLD  : return Pango::Weight::BOLD;
    case cydui::elements::attributes::FontWeight::HEAVY : return Pango::Weight::HEAVY;
    default                                             : return Pango::Weight::NORMAL;
  }
}

Pango::Style map_text_style(const cydui::elements::attributes::FontStyle& value) {
  switch (value) {
    case cydui::elements::attributes::FontStyle::NORMAL : return Pango::Style::NORMAL;
    case cydui::elements::attributes::FontStyle::ITALIC : return Pango::Style::ITALIC;
    case cydui::elements::attributes::FontStyle::OBLIQUE: return Pango::Style::OBLIQUE;
    default                                             : return Pango::Style::NORMAL;
  }
}

template <typename T>
void apply_stroke(
  const Context&                                      ctx,
  const cydui::elements::attributes::attrs_stroke<T>& stroke) {
  ctx->set_line_width(stroke._stroke_width);
  ctx->set_line_cap(map_line_cap(stroke._stroke_linecap));
  ctx->set_line_join(map_line_join(stroke._stroke_linejoin));
  ctx->set_dash(stroke._stroke_dasharray, stroke._stroke_dashoffset);
  ctx->set_miter_limit(stroke._stroke_miterlimit);
}

template <typename T>
void apply_fill(
  const Context&                                    ctx,
  const cydui::elements::attributes::attrs_fill<T>& fill) {
  ctx->set_fill_rule(map_fill_rule(fill._fill_rule));
}

void apply_paint_to_source(
  const Context&      ctx,
  const cydui::Paint& paint,
  double              opacity) {
  std::visit(
    [&]<typename T>(T&& it) {
      using paint_type = std::remove_cvref_t<T>;
      if constexpr (std::same_as<paint_type, cydui::paints::Solid>) {
        const cydui::paints::Solid& p = it;
        ctx->set_source_rgba(p.color.r, p.color.g, p.color.b, p.color.a * opacity);
      } else if constexpr (std::same_as<paint_type, cydui::paints::LinearGradient>) {
        const cydui::paints::LinearGradient& p  = it;
        auto                                 lg = Cairo::LinearGradient::create(p.x0, p.y0, p.x1, p.y1);
        for (auto& item: p.color_stops) {
          lg->add_color_stop_rgba(item.offset, item.color.r, item.color.g, item.color.b, item.color.a * opacity);
        }
        ctx->set_source(lg);
      } else if constexpr (std::same_as<paint_type, cydui::paints::RadialGradient>) {
        const cydui::paints::RadialGradient& p  = it;
        auto                                 lg = Cairo::RadialGradient::create(p.x0, p.y0, p.r0, p.x1, p.y1, p.r1);
        for (auto& item: p.color_stops) {
          lg->add_color_stop_rgba(item.offset, item.color.r, item.color.g, item.color.b, item.color.a * opacity);
        }
        ctx->set_source(lg);
      } else {
        LOG::print {WARN}("Paint {} is not supported by the Cairo renderer", refl::type_name<paint_type>);
      }
    },
    paint);
}

template <typename T>
void set_source_to_stroke(
  const Context&                                      ctx,
  const cydui::elements::attributes::attrs_stroke<T>& stroke) {
  apply_paint_to_source(ctx, stroke._stroke, stroke._stroke_opacity);
}

template <typename T>
void set_source_to_fill(
  const Context&                                    ctx,
  const cydui::elements::attributes::attrs_fill<T>& fill) {
  apply_paint_to_source(ctx, fill._fill, fill._fill_opacity);
}

double get_odd_offset(int thickness) {
  // return 0;
  return ((thickness % 2) != 0) ? 0.5 : 0.0;
}

export namespace cydui::platform::render {
  Cairo::Matrix to_cairo_matrix(const AffineTransform& transform) {
    Cairo::Matrix matrix;
    matrix.x0 = transform.translation[0];
    matrix.y0 = transform.translation[1];
    matrix.xx = transform.linear[0, 0];
    matrix.xy = transform.linear[0, 1];
    matrix.yx = transform.linear[1, 0];
    matrix.yy = transform.linear[1, 1];
    return matrix;
  }

  ELEMENT_PAINTER(Line) {
    PROF_SCOPE(Line);
    apply_stroke(ctx, element);
    set_source_to_stroke(ctx, element);

    double odd_offset = get_odd_offset(element._stroke_width);
    ctx->move_to(element.origin_x + element._x1 + odd_offset, element.origin_y + element._y1 + odd_offset);
    ctx->line_to(element.origin_x + element._x2 + odd_offset, element.origin_y + element._y2 + odd_offset);

    ctx->stroke();
  }

  ELEMENT_PAINTER(Arc) {
    PROF_SCOPE(Arc);
    ctx->save();
    apply_stroke(ctx, element);
    apply_fill(ctx, element);

    double odd_offset = get_odd_offset(element._stroke_width);
    auto   x          = element.origin_x + element._cx + odd_offset;
    auto   y          = element.origin_y + element._cy + odd_offset;
    ctx->save();
    ctx->translate(x, y);
    // editor->scale(_rx, _ry);
    if (element._include_center_point) {
      ctx->move_to(0, 0);
    }
    ctx->arc(
      0.0, 0.0, element._r, element._a1 * 2 * std::numbers::pi / 360.0, element._a2 * 2 * std::numbers::pi / 360.0);
    if (element._include_center_point) {
      ctx->close_path();
    }
    ctx->restore();

    set_source_to_fill(ctx, element);
    ctx->fill_preserve();

    set_source_to_stroke(ctx, element);
    ctx->stroke();

    ctx->restore();
  }

  ELEMENT_PAINTER(Circle) {
    PROF_SCOPE(Circle);
    ctx->save();
    apply_stroke(ctx, element);
    apply_fill(ctx, element);

    double odd_offset = get_odd_offset(element._stroke_width);
    ctx->begin_new_path();
    ctx->translate(element.origin_x + element._cx + odd_offset, element.origin_y + element._cy + odd_offset);
    ctx->arc(0.0, 0.0, element._r, 0, 2 * std::numbers::pi);
    set_source_to_fill(ctx, element);
    ctx->fill_preserve();

    set_source_to_stroke(ctx, element);
    ctx->stroke();
    ctx->restore();
  }

  ELEMENT_PAINTER(Ellipse) {
    PROF_SCOPE(Ellipse);
    ctx->save();
    apply_stroke(ctx, element);
    apply_fill(ctx, element);

    double odd_offset = get_odd_offset(element._stroke_width);
    ctx->begin_new_path();
    ctx->translate(element.origin_x + element._cx + odd_offset, element.origin_y + element._cy + odd_offset);
    ctx->scale(element._rx, element._ry);
    ctx->arc(0.0, 0.0, 1.0, 0.0, 2 * std::numbers::pi);
    set_source_to_fill(ctx, element);
    ctx->fill_preserve();

    set_source_to_stroke(ctx, element);
    ctx->stroke();
    ctx->restore();
  }

  ELEMENT_PAINTER(Polygon) {
    PROF_SCOPE(Polygon);
    apply_stroke(ctx, element);
    apply_fill(ctx, element);

    bool   first      = true;
    double odd_offset = get_odd_offset(element._stroke_width);
    for (const auto& p: element._points) {
      if (first) {
        ctx->move_to(element.origin_x + p[0] + odd_offset, element.origin_y + p[1] + odd_offset);
        first = false;
      } else {
        ctx->line_to(element.origin_x + p[0] + odd_offset, element.origin_y + p[1] + odd_offset);
      }
    }
    // Close the Polygon
    ctx->close_path();

    set_source_to_fill(ctx, element);
    ctx->fill_preserve();

    set_source_to_stroke(ctx, element);
    ctx->stroke();
  }

  ELEMENT_PAINTER(Polyline) {
    PROF_SCOPE(Polyline);
    apply_stroke(ctx, element);
    apply_fill(ctx, element);

    bool   first      = true;
    double odd_offset = get_odd_offset(element._stroke_width);
    for (const auto& p: element._points) {
      if (first) {
        ctx->move_to(element.origin_x + p[0] + odd_offset, element.origin_y + p[1] + odd_offset);
        first = false;
      } else {
        ctx->line_to(element.origin_x + p[0] + odd_offset, element.origin_y + p[1] + odd_offset);
      }
    }

    set_source_to_fill(ctx, element);
    ctx->fill_preserve();

    set_source_to_stroke(ctx, element);
    ctx->stroke();
  }

  ELEMENT_PAINTER(Rectangle) {
    PROF_SCOPE(Rectangle);
    apply_fill(ctx, element);

    auto x = element.origin_x + element._x;
    auto y = element.origin_y + element._y;
    // ctx->rectangle(x, y, element._w, element._h);

    ctx->move_to(x + element._border_radius_top_left.rx, y);
    ctx->line_to(x + element._w - element._border_radius_top_right.rx, y);
    if (element._border_radius_top_right.rx > 0 and element._border_radius_top_right.ry > 0) {
      ctx->save();
      ctx->translate(x + element._w - element._border_radius_top_right.rx, y + element._border_radius_top_right.ry);
      ctx->scale(element._border_radius_top_right.rx, element._border_radius_top_right.ry);
      ctx->arc(0, 0, 1, 3 * M_PI / 2, 2 * M_PI);
      ctx->restore();
    }
    ctx->line_to(x + element._w, y + element._h - element._border_radius_bottom_right.ry);
    if (element._border_radius_bottom_right.rx > 0 and element._border_radius_bottom_right.ry > 0) {
      ctx->save();
      ctx->translate(
        x + element._w - element._border_radius_bottom_right.rx,
        y + element._h - element._border_radius_bottom_right.ry);
      ctx->scale(element._border_radius_bottom_right.rx, element._border_radius_bottom_right.ry);
      ctx->arc(0, 0, 1, 0, M_PI / 2);
      ctx->restore();
    }
    ctx->line_to(x + element._border_radius_bottom_left.rx, y + element._h);
    if (element._border_radius_bottom_left.rx > 0 and element._border_radius_bottom_left.ry > 0) {
      ctx->save();
      ctx->translate(x + element._border_radius_bottom_left.rx, y + element._h - element._border_radius_bottom_left.ry);
      ctx->scale(element._border_radius_bottom_left.rx, element._border_radius_bottom_left.ry);
      ctx->arc(0, 0, 1, M_PI / 2, M_PI);
      ctx->restore();
    }
    ctx->line_to(x, y + element._border_radius_top_left.ry);
    if (element._border_radius_top_left.rx > 0 and element._border_radius_top_left.ry > 0) {
      ctx->save();
      ctx->translate(x + element._border_radius_top_left.rx, y + element._border_radius_top_left.ry);
      ctx->scale(element._border_radius_top_left.rx, element._border_radius_top_left.ry);
      ctx->arc(0, 0, 1, M_PI, 3 * M_PI / 2);
      ctx->restore();
    }

    ctx->close_path();
    set_source_to_fill(ctx, element);
    ctx->fill();

    double odd_offset = 0;
    // TOP BORDER
    odd_offset        = get_odd_offset(element._border_width_top);
    ctx->begin_new_path();
    if (element._border_radius_top_left.rx > 0 and element._border_radius_top_left.ry > 0) {
      ctx->save();
      ctx->translate(
        x + element._border_radius_top_left.rx + odd_offset, y + element._border_radius_top_left.ry + odd_offset);
      ctx->scale(element._border_radius_top_left.rx, element._border_radius_top_left.ry);
      ctx->arc(0, 0, 1, 5 * M_PI / 4, 3 * M_PI / 2);
      ctx->restore();
    } else {
      ctx->move_to(x + odd_offset, y + odd_offset);
    }
    ctx->line_to(x + element._w - element._border_radius_top_right.rx + odd_offset, y + odd_offset);
    if (element._border_radius_top_right.rx > 0 and element._border_radius_top_right.ry > 0) {
      ctx->save();
      ctx->translate(
        x + element._w - element._border_radius_top_right.rx + odd_offset,
        y + element._border_radius_top_right.ry + odd_offset);
      ctx->scale(element._border_radius_top_right.rx, element._border_radius_top_right.ry);
      ctx->arc(0, 0, 1, 3 * M_PI / 2, 7 * M_PI / 4);
      ctx->restore();
    }
    SET_BORDER_LINE(top);
    ctx->stroke();

    // RIGHT BORDER
    odd_offset = get_odd_offset(element._border_width_right);
    if (element._border_radius_top_right.rx > 0 and element._border_radius_top_right.ry > 0) {
      ctx->save();
      ctx->translate(
        x + element._w - element._border_radius_top_right.rx + odd_offset,
        y + element._border_radius_top_right.ry + odd_offset);
      ctx->scale(element._border_radius_top_right.rx, element._border_radius_top_right.ry);
      ctx->arc(0, 0, 1, 7 * M_PI / 4, 2 * M_PI);
      ctx->restore();
    } else {
      ctx->move_to(x + element._w + odd_offset, y + odd_offset);
    }
    ctx->line_to(x + element._w + odd_offset, y + element._h - element._border_radius_bottom_right.ry + odd_offset);
    if (element._border_radius_bottom_right.rx > 0 and element._border_radius_bottom_right.ry > 0) {
      ctx->save();
      ctx->translate(
        x + element._w - element._border_radius_bottom_right.rx + odd_offset,
        y + element._h - element._border_radius_bottom_right.ry + odd_offset);
      ctx->scale(element._border_radius_bottom_right.rx, element._border_radius_bottom_right.ry);
      ctx->arc(0, 0, 1, 0, M_PI / 4);
      ctx->restore();
    }
    SET_BORDER_LINE(right);
    ctx->stroke();

    // BOTTOM BORDER
    odd_offset = get_odd_offset(element._border_width_bottom);
    if (element._border_radius_bottom_right.rx > 0 and element._border_radius_bottom_right.ry > 0) {
      ctx->save();
      ctx->translate(
        x + element._w - element._border_radius_bottom_right.rx + odd_offset,
        y + element._h - element._border_radius_bottom_right.ry + odd_offset);
      ctx->scale(element._border_radius_bottom_right.rx, element._border_radius_bottom_right.ry);
      ctx->arc(0, 0, 1, M_PI / 4, M_PI / 2);
      ctx->restore();
    } else {
      ctx->move_to(x + element._w + odd_offset, y + element._h + odd_offset);
    }
    ctx->line_to(x + element._border_radius_bottom_left.rx + odd_offset, y + element._h + odd_offset);
    if (element._border_radius_bottom_left.rx > 0 and element._border_radius_bottom_left.ry > 0) {
      ctx->save();
      ctx->translate(
        x + element._border_radius_bottom_left.rx + odd_offset,
        y + element._h - element._border_radius_bottom_left.ry + odd_offset);
      ctx->scale(element._border_radius_bottom_left.rx, element._border_radius_bottom_left.ry);
      ctx->arc(0, 0, 1, M_PI / 2, 3 * M_PI / 4);
      ctx->restore();
    }
    SET_BORDER_LINE(bottom);
    ctx->stroke();

    // LEFT BORDER
    odd_offset = get_odd_offset(element._border_width_left);
    if (element._border_radius_bottom_left.rx > 0 and element._border_radius_bottom_left.ry > 0) {
      ctx->save();
      ctx->translate(
        x + element._border_radius_bottom_left.rx + odd_offset,
        y + element._h - element._border_radius_bottom_left.ry + odd_offset);
      ctx->scale(element._border_radius_bottom_left.rx, element._border_radius_bottom_left.ry);
      ctx->arc(0, 0, 1, 3 * M_PI / 4, M_PI);
      ctx->restore();
    } else {
      ctx->move_to(x + odd_offset, y + element._h + odd_offset);
    }
    ctx->line_to(x + odd_offset, y + element._border_radius_top_left.ry + odd_offset);
    if (element._border_radius_top_left.rx > 0 and element._border_radius_top_left.ry > 0) {
      ctx->save();
      ctx->translate(
        x + element._border_radius_top_left.rx + odd_offset, y + element._border_radius_top_left.ry + odd_offset);
      ctx->scale(element._border_radius_top_left.rx, element._border_radius_top_left.ry);
      ctx->arc(0, 0, 1, M_PI, 5 * M_PI / 4);
      ctx->restore();
    }
    SET_BORDER_LINE(left);
    ctx->stroke();
  }

  ELEMENT_PAINTER(Text) {
    PROF_SCOPE(Text);
    ctx->save();
    apply_fill(ctx, element);

    auto                   layout = Pango::Layout::create(ctx);
    Pango::FontDescription font {element._font_family};
    // font.set_family(element._font_family);
    font.set_size(element._font_size * Pango::SCALE);
    // font.set_stretch(map_text_stretch(element._font_stretch));
    // font.set_style(map_text_style(element._font_style));
    // font.set_weight(map_text_weight(element._font_weight));

    layout->set_font_description(font);
    layout->set_alignment(map_text_align(element._align));
    layout->set_justify(element._justify);

    if (element._w > 0) {
      layout->set_width(element._w * Pango::SCALE);
      layout->set_wrap(Pango::WrapMode::WORD_CHAR);
    } else {
      // layout->set_width(-1);
    }

    if (element._h > 0) {
      layout->set_height(element._h * Pango::SCALE);
      layout->set_wrap(Pango::WrapMode::WORD_CHAR);
    } else {
      // layout->set_width(-1);
    }

    layout->set_text(element._text);

    ctx->translate(element.origin_x + element._x + element._pivot_x, element.origin_y + element._y - element._pivot_y);
    ctx->rotate(element._rotate * std::numbers::pi / 180.0);
    ctx->scale(element._scale_x, element._scale_y);
    ctx->move_to(-element._pivot_x, element._pivot_y);
    set_source_to_fill(ctx, element);
    layout->show_in_cairo_context(ctx);

    ctx->restore();
  }

  ELEMENT_PAINTER(Pixelmap) {
    PROF_SCOPE(Pixelmap);
    // TODO - cairo_format_stride_for_width() should be called before allocating the image buffer
    // and thus use its output to determine the 'width' of the buffer.
    auto surf = Cairo::ImageSurface::create(
      (unsigned char*)element._pxm.data,
      Cairo::Surface::Format::ARGB32,
      (int)element._pxm.width(),
      (int)element._pxm.height(),
      (int)element._pxm.width() * 4);
    auto pattern = Cairo::SurfacePattern::create(surf);
    pattern->set_filter(Cairo::SurfacePattern::Filter::NEAREST);

    ctx->save();
    ctx->translate(element.origin_x + element._x + element._pivot_x, element.origin_y + element._y + element._pivot_y);
    ctx->rotate(element._rotate * std::numbers::pi / 180.0);
    ctx->scale((double)element._w / element._pxm.width(), (double)element._h / element._pxm.height());
    // ctx->set_antialias(Cairo::ANTIALIAS_NONE);
    ctx->set_source(pattern); //, -element._pivot_x, -element._pivot_y);
    ctx->paint_with_alpha(element._opacity);
    ctx->restore();

    surf->finish();
  }
} // namespace cydui::platform::render
