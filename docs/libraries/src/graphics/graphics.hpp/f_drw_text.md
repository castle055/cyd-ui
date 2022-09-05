---
layout: function
title: drw_text
owner: __MISSING__
brief: __MISSING__
tags:
  - function
defined_in_file: src/graphics/graphics.hpp
overloads:
  void drw_text(cydui::graphics::window_t *, layout::fonts::Font *, layout::color::Color *, std::string, int, int):
    arguments:
      - description: __OPTIONAL__
        name: win
        type: cydui::graphics::window_t *
      - description: __OPTIONAL__
        name: font
        type: layout::fonts::Font *
      - description: __OPTIONAL__
        name: color
        type: layout::color::Color *
      - description: __OPTIONAL__
        name: text
        type: std::string
      - description: __OPTIONAL__
        name: x
        type: int
      - description: __OPTIONAL__
        name: y
        type: int
    description: __OPTIONAL__
    return: __OPTIONAL__
    signature_with_names: void drw_text(cydui::graphics::window_t * win, layout::fonts::Font * font, layout::color::Color * color, std::string text, int x, int y)
namespace:
  - cydui
  - graphics
---
