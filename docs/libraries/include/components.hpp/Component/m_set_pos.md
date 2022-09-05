---
layout: method
title: set_pos
owner: __MISSING__
brief: __MISSING__
tags:
  - method
defined_in_file: include/components.hpp
overloads:
  cydui::components::Component * set_pos(cydui::components::Component *, IntProperty *, IntProperty *):
    arguments:
      - description: __OPTIONAL__
        name: relative
        type: cydui::components::Component *
      - description: __OPTIONAL__
        name: x
        type: IntProperty *
      - description: __OPTIONAL__
        name: y
        type: IntProperty *
    description: __MISSING__
    return: __OPTIONAL__
    signature_with_names: cydui::components::Component * set_pos(cydui::components::Component * relative, IntProperty * x, IntProperty * y)
  cydui::components::Component * set_pos(cydui::components::Component *, IntProperty::IntBinding, IntProperty::IntBinding):
    arguments:
      - description: __OPTIONAL__
        name: relative
        type: cydui::components::Component *
      - description: __OPTIONAL__
        name: x
        type: IntProperty::IntBinding
      - description: __OPTIONAL__
        name: y
        type: IntProperty::IntBinding
    description: __MISSING__
    return: __OPTIONAL__
    signature_with_names: cydui::components::Component * set_pos(cydui::components::Component * relative, IntProperty::IntBinding x, IntProperty::IntBinding y)
  cydui::components::Component * set_pos(cydui::components::Component *, int, int):
    arguments:
      - description: __OPTIONAL__
        name: relative
        type: cydui::components::Component *
      - description: __OPTIONAL__
        name: x
        type: int
      - description: __OPTIONAL__
        name: y
        type: int
    description: __MISSING__
    return: __OPTIONAL__
    signature_with_names: cydui::components::Component * set_pos(cydui::components::Component * relative, int x, int y)
---
