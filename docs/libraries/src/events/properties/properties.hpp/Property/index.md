---
layout: class
title: Property
owner: __MISSING__
brief: __MISSING__
tags:
  - class
defined_in_file: src/events/properties/properties.hpp
declaration: "\nclass Property;"
fields:
  _updating:
    annotation:
      - private
    description: __MISSING__
    type: bool
  binding:
    annotation:
      - protected
    description: __MISSING__
    type: std::function<void ()>
  dead:
    annotation:
      - private
    description: __MISSING__
    type: bool
  dependencies:
    annotation:
      - private
    description: __MISSING__
    type: std::unordered_set<Property *> *
  function_listeners:
    annotation:
      - private
    description: __MISSING__
    type: std::vector<std::function<void ()>> *
  listeners:
    annotation:
      - private
    description: __MISSING__
    type: std::unordered_set<Property *> *
  persistent:
    description: __MISSING__
    type: bool
  value:
    annotation:
      - protected
    description: __MISSING__
    type: void *
---
