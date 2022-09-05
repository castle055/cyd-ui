---
layout: method
title: set_size
owner: __MISSING__
brief: __MISSING__
tags:
  - method
defined_in_file: include/components.hpp
overloads:
  cydui::components::Component * set_size(IntProperty *, IntProperty *):
    arguments:
      - description: __OPTIONAL__
        name: w
        type: IntProperty *
      - description: __OPTIONAL__
        name: h
        type: IntProperty *
    description: __MISSING__
    return: __OPTIONAL__
    signature_with_names: cydui::components::Component * set_size(IntProperty * w, IntProperty * h)
  cydui::components::Component * set_size(IntProperty::IntBinding, IntProperty::IntBinding):
    arguments:
      - description: __OPTIONAL__
        name: w
        type: IntProperty::IntBinding
      - description: __OPTIONAL__
        name: h
        type: IntProperty::IntBinding
    description: __MISSING__
    return: __OPTIONAL__
    signature_with_names: cydui::components::Component * set_size(IntProperty::IntBinding w, IntProperty::IntBinding h)
  cydui::components::Component * set_size(int, int):
    arguments:
      - description: __OPTIONAL__
        name: w
        type: int
      - description: __OPTIONAL__
        name: h
        type: int
    description: __MISSING__
    return: __OPTIONAL__
    signature_with_names: cydui::components::Component * set_size(int w, int h)
---
