---
layout: method
title: set_pos
owner: __MISSING__
brief: __MISSING__
tags:
  - method
defined_in_file: src/layout/components/geometry/component_geometry.hpp
overloads:
  void set_pos(ComponentGeometry *, IntProperty *, IntProperty *):
    arguments:
      - description: __OPTIONAL__
        name: relative
        type: ComponentGeometry *
      - description: __OPTIONAL__
        name: x
        type: IntProperty *
      - description: __OPTIONAL__
        name: y
        type: IntProperty *
    description: __MISSING__
    return: __OPTIONAL__
    signature_with_names: void set_pos(ComponentGeometry * relative, IntProperty * x, IntProperty * y)
  void set_pos(ComponentGeometry *, IntProperty::IntBinding, IntProperty::IntBinding):
    arguments:
      - description: __OPTIONAL__
        name: relative
        type: ComponentGeometry *
      - description: __OPTIONAL__
        name: x
        type: IntProperty::IntBinding
      - description: __OPTIONAL__
        name: y
        type: IntProperty::IntBinding
    description: __MISSING__
    return: __OPTIONAL__
    signature_with_names: void set_pos(ComponentGeometry * relative, IntProperty::IntBinding x, IntProperty::IntBinding y)
  void set_pos(ComponentGeometry *, int, int):
    arguments:
      - description: __OPTIONAL__
        name: relative
        type: ComponentGeometry *
      - description: __OPTIONAL__
        name: x
        type: int
      - description: __OPTIONAL__
        name: y
        type: int
    description: __MISSING__
    return: __OPTIONAL__
    signature_with_names: void set_pos(ComponentGeometry * relative, int x, int y)
---
