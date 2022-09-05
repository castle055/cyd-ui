---
layout: method
title: Text
owner: __MISSING__
brief: __MISSING__
tags:
  - method
defined_in_file: include/primitives.hpp
is_ctor: true
overloads:
  Text(const primitives::Text &):
    arguments:
      - description: __OPTIONAL__
        name: unnamed-0
        type: const primitives::Text &
        unnamed: true
    description: __MISSING__
    return: __OPTIONAL__
    signature_with_names: Text(const primitives::Text &)
  Text(cydui::layout::color::Color *, layout::fonts::Font *, int, int, std::string):
    arguments:
      - description: __OPTIONAL__
        name: color
        type: cydui::layout::color::Color *
      - description: __OPTIONAL__
        name: font
        type: layout::fonts::Font *
      - description: __OPTIONAL__
        name: x
        type: int
      - description: __OPTIONAL__
        name: y
        type: int
      - description: __OPTIONAL__
        name: text
        type: std::string
    description: __MISSING__
    return: __OPTIONAL__
    signature_with_names: Text(cydui::layout::color::Color * color, layout::fonts::Font * font, int x, int y, std::string text)
  Text(primitives::Text &&):
    arguments:
      - description: __OPTIONAL__
        name: unnamed-0
        type: primitives::Text &&
        unnamed: true
    description: __MISSING__
    return: __OPTIONAL__
    signature_with_names: Text(primitives::Text &&)
---
