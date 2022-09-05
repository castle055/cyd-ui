---
layout: method
title: Rectangle
owner: __MISSING__
brief: __MISSING__
tags:
  - method
defined_in_file: include/primitives.hpp
is_ctor: true
overloads:
  Rectangle(const primitives::Rectangle &):
    arguments:
      - description: __OPTIONAL__
        name: unnamed-0
        type: const primitives::Rectangle &
        unnamed: true
    description: __MISSING__
    return: __OPTIONAL__
    signature_with_names: Rectangle(const primitives::Rectangle &)
  Rectangle(primitives::Rectangle &&):
    arguments:
      - description: __OPTIONAL__
        name: unnamed-0
        type: primitives::Rectangle &&
        unnamed: true
    description: __MISSING__
    return: __OPTIONAL__
    signature_with_names: Rectangle(primitives::Rectangle &&)
  explicit Rectangle(cydui::layout::color::Color *, int, int, int, int, bool):
    arguments:
      - description: __OPTIONAL__
        name: color
        type: cydui::layout::color::Color *
      - description: __OPTIONAL__
        name: x
        type: int
      - description: __OPTIONAL__
        name: y
        type: int
      - description: __OPTIONAL__
        name: w
        type: int
      - description: __OPTIONAL__
        name: h
        type: int
      - description: __OPTIONAL__
        name: filled
        type: bool
    description: __MISSING__
    return: __OPTIONAL__
    signature_with_names: explicit Rectangle(cydui::layout::color::Color * color, int x, int y, int w, int h, bool filled)
---
