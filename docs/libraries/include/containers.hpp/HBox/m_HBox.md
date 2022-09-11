---
layout: method
title: HBox
owner: __MISSING__
brief: __MISSING__
tags:
  - method
defined_in_file: include/containers.hpp
is_ctor: true
overloads:
  HBox(const containers::HBox &):
    arguments:
      - description: __OPTIONAL__
        name: unnamed-0
        type: const containers::HBox &
        unnamed: true
    description: __MISSING__
    return: __OPTIONAL__
    signature_with_names: HBox(const containers::HBox &)
  HBox(containers::HBox &&):
    arguments:
      - description: __OPTIONAL__
        name: unnamed-0
        type: containers::HBox &&
        unnamed: true
    description: __MISSING__
    return: __OPTIONAL__
    signature_with_names: HBox(containers::HBox &&)
  explicit HBox(containers::HBoxState *, int, std::function<void (cydui::components::Component *)>):
    arguments:
      - description: __OPTIONAL__
        name: _state
        type: containers::HBoxState *
      - description: __OPTIONAL__
        name: spacing
        type: int
      - description: __OPTIONAL__
        name: IN
        type: std::function<void (cydui::components::Component *)>
    description: __MISSING__
    return: __OPTIONAL__
    signature_with_names: explicit HBox(containers::HBoxState * _state, int spacing, std::function<void (cydui::components::Component *)> IN)
---
