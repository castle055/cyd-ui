// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include <pangomm.h>
#include <pangomm/cairofontmap.h>
#include <tracy/Tracy.hpp>

export module cydui.elements.Text;

import std;

export import cydui.dimensions;
export import cydui.elements.attributes;
export import cydui.elements.footprint;

Pango::Alignment map_text_align(const cydui::elements::attributes::TextAlign& value) {
  switch (value) {
    case cydui::elements::attributes::TextAlign::LEFT  : return Pango::Alignment::LEFT;
    case cydui::elements::attributes::TextAlign::CENTER: return Pango::Alignment::CENTER;
    case cydui::elements::attributes::TextAlign::RIGHT : return Pango::Alignment::RIGHT;
    default                                            : return Pango::Alignment::LEFT;
  }
}

Glib::RefPtr<Pango::Context> _context {nullptr};

export namespace cydui {
  struct TextExtents {
    struct {
      int x, y, w, h;
    } ink {};

    struct {
      int x, y, w, h;
    } logical {};
  };
} // namespace cydui

export namespace cydui::elements {
  // using font_extents_t = Cairo::FontExtents;
  // using Text_extents_t = Cairo::TextExtents;
  //
  struct Text: attributes::attrs_core<Text>,
               attributes::attrs_fill<Text>,
               attributes::attrs_font<Text>,
               attributes::attr_align<Text>,
               attributes::attr_justify<Text>,
               attributes::attr_x<Text>,
               attributes::attr_y<Text>,
               attributes::attr_rotate<Text>,
               attributes::attr_pivot_x<Text>,
               attributes::attr_pivot_y<Text>,
               attributes::attr_scale_x<Text>,
               attributes::attr_scale_y<Text>,
               attributes::attr_w<Text>,
               attributes::attr_h<Text> {
    std::string _text;
    explicit Text(const std::string& text)
        : _text(text) {
      if (_context == nullptr) {
        _context = Pango::CairoFontMap::get_default()->create_context();
      }
      // TracyAllocN(this, sizeof(decltype(*this)), "fragment_elements");
    }
    ~Text() {
      // TracyFreeN(this, "fragment_elements");
    }
    Footprint get_footprint() const {
      auto                   layout = Pango::Layout::create(_context);
      Pango::FontDescription font {_font_family};
      // font.set_family(element._font_family);
      font.set_size(_font_size * Pango::SCALE);
      // font.set_stretch(map_text_stretch(element._font_stretch));
      // font.set_style(map_text_style(element._font_style));
      // font.set_weight(map_text_weight(element._font_weight));

      layout->set_font_description(font);
      layout->set_alignment(map_text_align(_align));
      layout->set_justify(_justify);

      if (_w > 0) {
        layout->set_width(_w * Pango::SCALE);
        layout->set_wrap(Pango::WrapMode::WORD_CHAR);
      } else {
        // layout->set_width(-1);
      }

      if (_h > 0) {
        layout->set_height(_h * Pango::SCALE);
        layout->set_wrap(Pango::WrapMode::WORD_CHAR);
      } else {
        // layout->set_width(-1);
      }

      layout->set_text(_text);

      int w, h;
      layout->get_pixel_size(w, h);

      return {
        .x = static_cast<int>(_x),
        .y = static_cast<int>(_y),
        .w = static_cast<int>(w * _scale_x),
        .h = static_cast<int>(h * _scale_y),
      };
    }

    TextExtents get_extents() const {
      auto                   layout = Pango::Layout::create(_context);
      Pango::FontDescription font {_font_family};
      // font.set_family(element._font_family);
      font.set_size(_font_size * Pango::SCALE);
      // font.set_stretch(map_text_stretch(element._font_stretch));
      // font.set_style(map_text_style(element._font_style));
      // font.set_weight(map_text_weight(element._font_weight));

      layout->set_font_description(font);
      layout->set_alignment(map_text_align(_align));
      layout->set_justify(_justify);

      if (_w > 0) {
        layout->set_width(_w * Pango::SCALE);
        layout->set_wrap(Pango::WrapMode::WORD_CHAR);
      } else {
        // layout->set_width(-1);
      }

      if (_h > 0) {
        layout->set_height(_h * Pango::SCALE);
        layout->set_wrap(Pango::WrapMode::WORD_CHAR);
      } else {
        // layout->set_width(-1);
      }

      layout->set_text(_text);

      Pango::Rectangle ink, logical;
      layout->get_pixel_extents(ink, logical);

      return {
        .ink     = {    .x = ink.get_x(),     .y = ink.get_y(),     .w = ink.get_width(),     .h = ink.get_height()},
        .logical = {.x = logical.get_x(), .y = logical.get_y(), .w = logical.get_width(), .h = logical.get_height()}
      };
    }
  };
} // namespace cydui::elements
