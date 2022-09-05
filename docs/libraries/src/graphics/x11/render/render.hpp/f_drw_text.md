---
layout: function
title: drw_text
owner: __MISSING__
brief: __MISSING__
tags:
  - function
defined_in_file: src/graphics/x11/render/render.hpp
overloads:
  void drw_text(cydui::graphics::window_t *, window_font, cydui::layout::color::Color *, std::string, int, int):
    arguments:
      - description: __OPTIONAL__
        name: win
        type: cydui::graphics::window_t *
      - description: __OPTIONAL__
        name: font
        type: window_font
      - description: __OPTIONAL__
        name: color
        type: cydui::layout::color::Color *
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
    signature_with_names: void drw_text(cydui::graphics::window_t * win, window_font font, cydui::layout::color::Color * color, std::string text, int x, int y)
namespace:
  - render
---
