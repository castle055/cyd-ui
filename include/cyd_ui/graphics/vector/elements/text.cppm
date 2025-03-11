// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include <cairomm-1.16/cairomm/cairomm.h>
#include <tracy/Tracy.hpp>

export module cydui.graphics.vector.elements.text;

import std;

export import cydui.dimensions;
export import cydui.graphics.vector.element;

export namespace vg {
  using font_extents_t = Cairo::FontExtents;
  using text_extents_t = Cairo::TextExtents;

  struct text: element_t,
               attrs_core<text>,
               attrs_fill<text>,
               // attrs_stroke<text>,
               attrs_font<text>,
               attr_x<text>,
               attr_y<text>,
               attr_rotate<text>,
               attr_pivot_x<text>,
               attr_pivot_y<text>,
               attr_scale_x<text>,
               attr_scale_y<text>,
               attr_w<text>,
               attr_h<text> {
    std::string _text;
    explicit text(std::string _text)
        : _text(std::move(_text)) {
      // TracyAllocN(this, sizeof(decltype(*this)), "fragment_elements");
    }
    ~text() override {
      // TracyFreeN(this, "fragment_elements");
    }
    void apply_to(pixelmap_editor_t& editor) const override {
      apply_font(editor);
      // apply_stroke(editor);
      apply_fill(editor);

      Cairo::FontExtents fextents;
      editor->get_font_extents(fextents);

      Cairo::TextExtents extents;
      editor->get_text_extents(_text, extents);

      editor->save();
      editor->translate(origin_x + _x + _pivot_x, origin_y + _y - _pivot_y);
      editor->rotate(_rotate * std::numbers::pi / 180.0);
      editor->scale(_scale_x, _scale_y);
      editor->move_to(-_pivot_x, _pivot_y);
      set_source_to_fill(editor);
      editor->show_text(_text);
      editor->restore();

      // editor->move_to(origin_x + _x, origin_y + _y + fextents.ascent);
      // editor->show_text(_text);
      // editor->move_to(origin_x + _x + 5, origin_y + _y + fextents.height);
      // editor->show_text(_text);
      // editor->move_to(origin_x + _x + 10, origin_y + _y + extents.height);
      // editor->show_text(_text);
    }

    footprint get_footprint() const override {
      pixelmap_t        pm{0, 0};
      pixelmap_editor_t pe{pm};

      apply_font(pe);
      apply_fill(pe);

      Cairo::FontExtents fextents;
      pe->get_font_extents(fextents);

      Cairo::TextExtents extents;
      pe->get_text_extents(_text, extents);

      return {
        .x = static_cast<int>(_x),
        .y = static_cast<int>(_y - fextents.ascent),
        .w = static_cast<int>(extents.x_advance),
        .h = static_cast<int>(fextents.height),
      };
    }

    font_extents_t get_font_extents() const {
      pixelmap_t        pm{0, 0};
      pixelmap_editor_t pe{pm};

      apply_font(pe);

      Cairo::FontExtents fextents;
      pe->get_font_extents(fextents);

      return fextents;
    }

    text_extents_t get_text_extents() const {
      pixelmap_t        pm{0, 0};
      pixelmap_editor_t pe{pm};

      apply_font(pe);

      Cairo::TextExtents extents;
      pe->get_text_extents(_text, extents);

      return extents;
    }
  };
} // namespace vg
