---
layout: class
title: Component
owner: __MISSING__
brief: __MISSING__
tags:
  - class
defined_in_file: include/components.hpp
declaration: "\nclass cydui::components::Component;"
fields:
  children:
    description: __MISSING__
    type: std::vector<Component *>
  inner_redraw:
    annotation:
      - private
    description: __MISSING__
    type: std::function<void (Component *)>
  parent:
    annotation:
      - private
    description: __MISSING__
    type: cydui::components::Component *
  state:
    description: __MISSING__
    type: cydui::components::ComponentState *
namespace:
  - cydui
  - components
---
