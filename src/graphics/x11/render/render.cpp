// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

#include "../state/state.hpp"
#include "render.hpp"

#include <iostream>

#include <X11/Xlib.h>

import fabric.logging;

using namespace x11;

void render_sbr(cyd::ui::graphics::window_t* win, XImage* image) {
  std::lock_guard lk{win->render_mtx};
  image->width = win->render_target->width();
  image->height = win->render_target->height();
  image->xoffset = 0;
  image->format = ZPixmap;
  image->data = (char*) win->render_target->data;
  image->byte_order = LSBFirst;
  image->bitmap_unit = 32;
  image->bitmap_bit_order = MSBFirst;
  image->bitmap_pad = 8;
  image->depth = 32; //DisplayPlanes(state::get_dpy(), state::get_screen());
  image->bits_per_pixel = 32;
  image->bytes_per_line = (int) win->render_target->width() * 4;
  image->red_mask = (0xff) << 24;
  image->green_mask = (0xff) << 16;
  image->blue_mask = (0xff) << 8;

  auto _pev = win->profiler->scope_event("render::render_sbr");
  XPutImage(
    state::get_dpy(),
    win->xwin,
    win->gc,
    image,
    0,
    0,
    0,
    0,
    win->render_target->width(),
    win->render_target->height()
  );
}

using namespace std::chrono_literals;

static std::unordered_map<u32, XColor> xcolor_cache;

struct xcolor_hot_cache_entry_t {
  u32 id;
  XColor xcolor;
};
static constexpr u32 xcolor_hot_cache_size = 8;
static xcolor_hot_cache_entry_t xcolor_hot_cache[xcolor_hot_cache_size];
static u32 xcolor_hot_cache_current_insert_index = 0U;

XColor color_to_xcolor(color::Color color) {
  u32 color_id = color.to_id();

  for (auto &entry: xcolor_hot_cache) {
    if (entry.id == color_id) {
      return entry.xcolor;
    }
  }

  if (xcolor_cache.contains(color_id)) {
    auto &c = xcolor_cache[color_id];
    xcolor_hot_cache[xcolor_hot_cache_current_insert_index++] = {
      .id = color_id,
      .xcolor = c,
    };
    if (xcolor_hot_cache_current_insert_index >= xcolor_hot_cache_size) {
      xcolor_hot_cache_current_insert_index = 0U;
    }
    return c;
  }

  auto dpy = state::get_dpy();
  auto screen = state::get_screen();
  Colormap map = DefaultColormap(dpy, screen);
  XColor c;
  XParseColor(dpy, map, color.to_string().c_str(), &c);
  XAllocColor(dpy, map, &c);

  xcolor_hot_cache[xcolor_hot_cache_current_insert_index++] = {
    .id = color_id,
    .xcolor = c,
  };
  if (xcolor_hot_cache_current_insert_index >= xcolor_hot_cache_size) {
    xcolor_hot_cache_current_insert_index = 0U;
  }

  xcolor_cache[color_id] = c;
  return c;
}

static std::unordered_map<u32, XftColor*> xftcolor_cache;

XftColor* color_to_xftcolor(color::Color color) {
  u32 color_id = color.to_id();
  if (xftcolor_cache.contains(color_id))
    return xftcolor_cache[color_id];

  auto* c = new XftColor;

  auto dpy = state::get_dpy();
  auto screen = state::get_screen();
  if (!XftColorAllocName(
    dpy,
    DefaultVisual(dpy, screen),
    DefaultColormap(dpy, screen),
    color.to_string().c_str(),
    c
  )) {
    LOG::print{ERROR}("Cannot allocate color {}", color.to_string());
  }

  xftcolor_cache[color_id] = c;
  return c;
}

void render::resize(pixelmap_t* target, int w, int h) {
  //if (w != target->w || h != target->h) {
  //  Drawable new_drw = XCreatePixmap(state::get_dpy(),
  //    target->win->xwin,
  //    w,
  //    h,
  //    DefaultDepth(state::get_dpy(), state::get_screen()));
  //  XSetForeground(state::get_dpy(),
  //    target->gc,
  //    BlackPixel(state::get_dpy(), state::get_screen()));
  //  XFillRectangle(state::get_dpy(), new_drw, target->gc, 0, 0, w, h);
  //
  //  //target->win->x_mtx.lock();
  //  XCopyArea(state::get_dpy(),
  //    target->drawable,
  //    new_drw,
  //    target->gc,
  //    0,
  //    0,
  //    target->w,
  //    target->h,
  //    0,
  //    0);
  //
  //  XFreePixmap(state::get_dpy(), target->drawable);
  //  XFreeGC(state::get_dpy(), target->gc);
  //
  //  target->drawable = new_drw;
  //  target->gc = XCreateGC(state::get_dpy(), target->drawable, 0, NULL);
  //
  //  //target->win->x_mtx.unlock();
  //
  //  XFlush(state::get_dpy());
  //
  //  target->w = w;
  //  target->h = h;
  //  render::flush(target->win);
  //}
}

void render::flush(cyd::ui::graphics::window_t* win) {
  auto _pev = win->profiler->scope_event("render::flush");
  XImage ximg;
  XInitImage(&ximg);
  auto* tmp = win->render_target;
  win->render_target = win->staging_target;
  win->staging_target = tmp;
  render_sbr(win, &ximg);
}
