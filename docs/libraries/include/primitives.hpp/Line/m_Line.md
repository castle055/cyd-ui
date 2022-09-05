---
layout: method
title: Line
owner: __MISSING__
brief: __MISSING__
tags:
  - method
defined_in_file: include/primitives.hpp
is_ctor: true
overloads:
  Line(const primitives::Line &):
    arguments:
      - description: __OPTIONAL__
        name: unnamed-0
        type: const primitives::Line &
        unnamed: true
    description: __MISSING__
    return: __OPTIONAL__
    signature_with_names: Line(const primitives::Line &)
  Line(primitives::Line &&):
    arguments:
      - description: __OPTIONAL__
        name: unnamed-0
        type: primitives::Line &&
        unnamed: true
    description: __MISSING__
    return: __OPTIONAL__
    signature_with_names: Line(primitives::Line &&)
  explicit Line(cydui::layout::color::Color *, int, int, int, int):
    arguments:
      - description: __OPTIONAL__
        name: color
        type: cydui::layout::color::Color *
      - description: __OPTIONAL__
        name: x1
        type: int
      - description: __OPTIONAL__
        name: y1
        type: int
      - description: __OPTIONAL__
        name: x2
        type: int
      - description: __OPTIONAL__
        name: y2
        type: int
    description: __MISSING__
    return: __OPTIONAL__
    signature_with_names: explicit Line(cydui::layout::color::Color * color, int x1, int y1, int x2, int y2)
---
