---
layout: method
title: Arc
owner: __MISSING__
brief: __MISSING__
tags:
  - method
defined_in_file: include/primitives.hpp
is_ctor: true
overloads:
  Arc(const primitives::Arc &):
    arguments:
      - description: __OPTIONAL__
        name: unnamed-0
        type: const primitives::Arc &
        unnamed: true
    description: __MISSING__
    return: __OPTIONAL__
    signature_with_names: Arc(const primitives::Arc &)
  Arc(primitives::Arc &&):
    arguments:
      - description: __OPTIONAL__
        name: unnamed-0
        type: primitives::Arc &&
        unnamed: true
    description: __MISSING__
    return: __OPTIONAL__
    signature_with_names: Arc(primitives::Arc &&)
  explicit Arc(cydui::layout::color::Color *, int, int, int, int, int, int, bool):
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
        name: a0
        type: int
      - description: __OPTIONAL__
        name: a1
        type: int
      - description: __OPTIONAL__
        name: filled
        type: bool
    description: __MISSING__
    return: __OPTIONAL__
    signature_with_names: explicit Arc(cydui::layout::color::Color * color, int x, int y, int w, int h, int a0, int a1, bool filled)
---
