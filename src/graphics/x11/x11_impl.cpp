// Copyright (c) 2024, Victor Castillo, All rights reserved.

//
// Created by castle on 8/21/22.
//

#include <cyd_fabric/logging/logging.hpp>
#include "images.h"
#include "events/events.hpp"
#include <X11/Xft/Xft.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <png.h>
#include "state/state.hpp"
#include "render/render.hpp"

using namespace x11;

const logging::logger log_task = {.name = "X11_IMPL", .on = true};

char* log_error_code(int err) {
  static char buf[128];
  XGetErrorText(state::get_dpy(), err, buf, 128);
  return buf;
}

static Bool evpredicate() {
  return True;
}

static int geom_mask_to_gravity(int mask) {
  switch (mask & (XNegative | YNegative)) {
    case 0:
      return NorthWestGravity;
    case XNegative:
      return NorthEastGravity;
    case YNegative:
      return SouthWestGravity;
  }
  
  return SouthEastGravity;
}

static std::unordered_map<unsigned long, window_t*> window_map {};

cyd::ui::graphics::window_t* cyd::ui::graphics::create_window(
  cyd::fabric::async::async_bus_t* async_bus,
  prof::context_t* profiler,
  const char* title,
  const char* wclass,
  int x,
  int y,
  int w,
  int h,
  bool override_redirect
) {
  static int _ig = XInitThreads();
  
  XVisualInfo vinfo;
  if (not XMatchVisualInfo(state::get_dpy(), state::get_screen(), 32, TrueColor, &vinfo)) {
    log_task.error("XMatchVisualInfo failed to find a Visual");
  }
  Colormap cmap = XCreateColormap(state::get_dpy(), state::get_root(), vinfo.visual, AllocNone);
  XSetWindowAttributes wa = {
    .background_pixel = 0,//ParentRelative,
    .border_pixel = 0,
    .bit_gravity = NorthWestGravity,
    .event_mask  = FocusChangeMask | KeyPressMask | KeyReleaseMask
      | VisibilityChangeMask | StructureNotifyMask | ButtonMotionMask
      | ButtonPressMask | ButtonReleaseMask | ExposureMask | FocusChangeMask
      | LeaveWindowMask | EnterWindowMask | PointerMotionMask,
    .override_redirect =
    override_redirect, // This makes it immutable across workspaces
    .colormap = cmap,
  };
  str title_str = title;
  str wclass_str = wclass;
  XClassHint ch = {title_str.data(), wclass_str.data()};
  
  int x_o, y_o;
  unsigned int w_o, h_o;
  str geom;
  geom += "0x0";
  if (x >= 0)
    geom += "+";
  geom += std::to_string(x);
  if (y >= 0)
    geom += "+";
  geom += std::to_string(y);
  
  int gm_mask = XParseGeometry(geom.c_str(), &x_o, &y_o, &w_o, &h_o);
  
  if (gm_mask & XNegative)
    x_o += DisplayWidth(state::get_dpy(), state::get_screen()) - w;// - 2;
  if (gm_mask & YNegative)
    y_o += DisplayHeight(state::get_dpy(), state::get_screen()) - h;// - 2;
  
  Window xwin = XCreateWindow(state::get_dpy(),
    state::get_root(),
    x_o,
    y_o,
    w,
    h,
    0,
    vinfo.depth, //DefaultDepth(state::get_dpy(), state::get_screen()),
    InputOutput,
    vinfo.visual, //DefaultVisual(state::get_dpy(), state::get_screen()),
    CWBorderPixel | CWBitGravity | CWColormap | CWBackPixel | CWEventMask
      | (override_redirect ? CWOverrideRedirect : 0U),
    &wa);
  XSetClassHint(state::get_dpy(), xwin, &ch);
  XStoreName(state::get_dpy(), xwin, title);
  XSync(state::get_dpy(), False);
  
  log_task.info(
    "Created window %lX at (%s) x: %d, y: %d", xwin, geom.c_str(), x, y);
  
  if (!override_redirect) {
    XWMHints wm = {.flags = InputHint, .input = 1};
    XSizeHints* sizeh;
    sizeh = XAllocSizeHints();
    sizeh->flags = PSize | PResizeInc | PBaseSize | PMinSize;
    sizeh->height = h;
    sizeh->width = w;
    sizeh->height_inc = 1;
    sizeh->width_inc = 1;
    if (gm_mask & (XValue | YValue)) {
      sizeh->flags |= USPosition | PWinGravity;
      sizeh->x = x_o;
      sizeh->y = y_o;
      sizeh->win_gravity = geom_mask_to_gravity(gm_mask);
    }
    //
    XSetWMProperties(
      state::get_dpy(), xwin, NULL, NULL, NULL, 0, sizeh, &wm, &ch);
    XFree(sizeh);
  }
  XSync(state::get_dpy(), False);
  
  //  XDefineCursor(state::get_dpy(), xwin, state::cursor[CurNormal]->cursor);
  if (override_redirect) {
    XMapRaised(state::get_dpy(), xwin);
  } else {
    XMapWindow(state::get_dpy(), xwin);
  }
  log_task.debug("Mapping window %lX", xwin);
  
  auto* win = new window_t(async_bus, profiler, xwin, w, h);
  window_map[get_id(win)] = win;
  
  win->gc = XCreateGC(state::get_dpy(), xwin, 0, nullptr);
  
  XSync(state::get_dpy(), False);
  
  x11::events::start();
  
  return win;
}

window_t::window_t(
  cyd::fabric::async::async_bus_t* async_bus,
  prof::context_t* profiler,
  Window xwin,
  unsigned long w,
  unsigned long h
) {
  this->bus = async_bus;
  this->profiler = profiler;
  this->xwin = xwin;
  this->staging_target = new pixelmap_t {w, h};
  this->render_target = new pixelmap_t {w, h};
  render::start(this);
}

void cyd::ui::graphics::resize(window_t* win, int w, int h) {
  if (w == 0 || h == 0) return;
  std::lock_guard lk {win->render_mtx};
  win->render_target->resize({(size_t) w, (size_t) h});
  win->staging_target->resize({(size_t) w, (size_t) h});
}

void cyd::ui::graphics::flush(window_t* win) {
  render::flush(win);
}

static str to_pattern(font::Font* font) {
  str str;
  str.append(font->name + ":");
  str.append("size=" + std::to_string(font->size) + ":");
  str.append("antialias=");
  str.append((font->antialias ? "true" : "false"));
  str.append(":");
  str.append("autohint=");
  str.append((font->autohint ? "true" : "false"));
  //str.append(":");
  
  return str;
}

static window_font load_font(
  cyd::ui::graphics::window_t* win, font::Font* font
) {
  str font_spec = to_pattern(font);
  if (win->loaded_fonts.contains(font_spec))
    return win->loaded_fonts[font_spec];
  XftFont* xfont;
  FcPattern* pattern;
  
  if (!(xfont = XftFontOpenName(
    state::get_dpy(), state::get_screen(), font_spec.c_str()))) {
    log_task.error("Cannot load font from name %s", font_spec.c_str());
    return {};
  }
  if (!(pattern = FcNameParse((FcChar8*) (font_spec.c_str())))) {
    log_task.error("Cannot parse font name to pattern: %s", font_spec.c_str());
    return {};
  }
  
  auto f = window_font {.xfont = xfont, .pattern = pattern};
  win->loaded_fonts[font_spec] = f;
  return f;
}

static void unload_font(
  cyd::ui::graphics::window_t* win, font::Font* font
) {
  // TODO - Implement
}

static window_image load_image(
  cyd::ui::graphics::window_t* win, cyd::ui::graphics::images::image_t* img
) {
  if (win->loaded_images.contains(img->path))
    return win->loaded_images[img->path];
  
  XImage* image = nullptr;
  
  //if (img->path.ends_with(".jpg")
  //  || img->path.ends_with("jpeg")) {
  imgs::img_data data = imgs::read_jpg(img->path);
  
  image = XCreateImage(state::get_dpy(),
    CopyFromParent,
    DisplayPlanes(state::get_dpy(), state::get_screen()),
    ZPixmap,
    0,
    data.data,
    data.width,
    data.height,
    8,
    data.width * data.components);
  //log_task.info("STATUS = %d", XInitImage(image));
  //}
  
  //
  //auto f = window_font {.xfont = xfont, .pattern = pattern};
  //win->loaded_fonts[font_spec] = f;
  auto i = window_image {image};
  win->loaded_images[img->path] = i;
  return i;
}

static void unload_image(
  cyd::ui::graphics::window_t* win,
  cyd::ui::graphics::images::image_t* img
) {
  // TODO - Implement
}

static std::unordered_map<str, XftFont*> cached_fonts;

std::pair<int, int> get_text_size(
  font::Font* font, const str &text
) {
  str font_spec = to_pattern(font);
  XftFont* xfont;
  if (cached_fonts.contains(font_spec)) {
    xfont = cached_fonts[font_spec];
  } else {
    if (!(xfont = XftFontOpenName(
      state::get_dpy(), state::get_screen(), font_spec.c_str()))) {
      log_task.error("Cannot load font from name %s", font_spec.c_str());
      return {};
    }
    cached_fonts[font_spec] = xfont;
  }
  XGlyphInfo x_glyph_info;
  XftTextExtentsUtf8(state::get_dpy(),
    xfont,
    (XftChar8*) text.c_str(),
    (int) text.size(),
    &x_glyph_info);
  //XftFontClose(state::get_dpy(), xfont);
  return {x_glyph_info.xOff, x_glyph_info.y};
}

std::pair<int, int> get_image_size(
  cyd::ui::graphics::images::image_t* img
) {
  imgs::img_data data = imgs::read_jpg(img->path);
  return {data.width, data.height};
}

pixelmap_t* cyd::ui::graphics::get_frame(cyd::ui::graphics::window_t* win) {
  return win->staging_target;
}

unsigned long cyd::ui::graphics::get_id(cyd::ui::graphics::window_t* win) {
  return (unsigned int) win->xwin;
}
std::optional<window_t*> cyd::ui::graphics::get_from_id(unsigned long id) {
  return window_map.contains(id) ? std::optional {window_map[id]} : std::nullopt;
}

std::pair<int, int> cyd::ui::graphics::get_position(cyd::ui::graphics::window_t* win) {
  XWindowAttributes attrs;
  XGetWindowAttributes(state::get_dpy(), win->xwin, &attrs);
  return {attrs.x, attrs.y};
}
std::pair<int, int> cyd::ui::graphics::get_size(cyd::ui::graphics::window_t* win) {
  XWindowAttributes attrs;
  XGetWindowAttributes(state::get_dpy(), win->xwin, &attrs);
  return {attrs.width, attrs.height};
}

void cyd::ui::graphics::terminate(cyd::ui::graphics::window_t* win) {
  XUnmapWindow(state::get_dpy(), win->xwin);
  XDestroyWindow(state::get_dpy(), win->xwin);
  delete win->staging_target;
  delete win->render_target;
  win->render_thd->running = false;
  win->render_thd->join();
  
  delete win;
}