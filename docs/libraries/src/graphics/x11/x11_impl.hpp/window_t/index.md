---
layout: class
title: window_t
owner: __MISSING__
brief: __MISSING__
tags:
  - class
defined_in_file: src/graphics/x11/x11_impl.hpp
declaration: "\nstruct window_t;"
ctor: unspecified
dtor: unspecified
fields:
  dirty:
    description: __MISSING__
    type: bool
  drawable:
    description: __MISSING__
    type: Drawable
  gc:
    description: __MISSING__
    type: GC
  h:
    description: __MISSING__
    type: int
  loaded_fonts:
    description: __MISSING__
    type: loaded_font_map_t
  render_mtx:
    description: __MISSING__
    type: std::mutex
  render_reqs:
    description: __MISSING__
    type: std::deque<window_render_req>
  render_thd:
    description: __MISSING__
    type: cydui::threading::thread_t *
  w:
    description: __MISSING__
    type: int
  x_mtx:
    description: __MISSING__
    type: std::mutex
  xwin:
    description: __MISSING__
    type: Window
---
