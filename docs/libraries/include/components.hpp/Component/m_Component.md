---
layout: method
title: Component
owner: __MISSING__
brief: __MISSING__
tags:
  - method
defined_in_file: include/components.hpp
is_ctor: true
overloads:
  Component():
    description: __MISSING__
    return: __OPTIONAL__
    signature_with_names: Component()
  Component(const cydui::components::Component &):
    arguments:
      - description: __OPTIONAL__
        name: unnamed-0
        type: const cydui::components::Component &
        unnamed: true
    description: __MISSING__
    return: __OPTIONAL__
    signature_with_names: Component(const cydui::components::Component &)
  Component(cydui::components::ComponentState *):
    arguments:
      - description: __OPTIONAL__
        name: state
        type: cydui::components::ComponentState *
    description: __MISSING__
    return: __OPTIONAL__
    signature_with_names: Component(cydui::components::ComponentState * state)
  Component(cydui::components::ComponentState *, std::function<void (Component *)>):
    arguments:
      - description: __OPTIONAL__
        name: state
        type: cydui::components::ComponentState *
      - description: __OPTIONAL__
        name: inner
        type: std::function<void (Component *)>
    description: __MISSING__
    return: __OPTIONAL__
    signature_with_names: Component(cydui::components::ComponentState * state, std::function<void (Component *)> inner)
  Component(std::function<void (Component *)>):
    arguments:
      - description: __OPTIONAL__
        name: inner
        type: std::function<void (Component *)>
    description: __MISSING__
    return: __OPTIONAL__
    signature_with_names: Component(std::function<void (Component *)> inner)
---
