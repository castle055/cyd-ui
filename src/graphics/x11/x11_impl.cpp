//
// Created by castle on 8/21/22.
//

#include "x11_impl.hpp"
#include "../events.hpp"
#include "graphics.hpp"
#include "images.h"
#include "render/render.hpp"
#include "state/state.hpp"
#include <X11/Xft/Xft.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <png.h>

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
    case 0: return NorthWestGravity;
    case XNegative: return NorthEastGravity;
    case YNegative: return SouthWestGravity;
  }
  
  return SouthEastGravity;
}

cydui::graphics::window_t* cydui::graphics::create_window(
  const char* title,
  const char* wclass,
  int x,
  int y,
  int w,
  int h,
  bool override_redirect
) {
  static int _ig = XInitThreads();
  
  XSetWindowAttributes wa = {
    .background_pixmap =
    BlackPixel(state::get_dpy(), state::get_screen()),//ParentRelative,
    .bit_gravity = NorthWestGravity,
    .event_mask  = FocusChangeMask | KeyPressMask | KeyReleaseMask
      | VisibilityChangeMask | StructureNotifyMask | ButtonMotionMask
      | ButtonPressMask | ButtonReleaseMask | ExposureMask
      | LeaveWindowMask | EnterWindowMask | PointerMotionMask,
    .override_redirect =
    override_redirect// This makes it immutable across workspaces
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
    DefaultDepth(state::get_dpy(), state::get_screen()),
    InputOutput,
    DefaultVisual(state::get_dpy(), state::get_screen()),
    CWBorderPixel | CWBitGravity | CWColormap | CWBackPixmap | CWEventMask
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
  
  //XSync(state::get_dpy(), False);
  
  auto* win = new window_t(xwin, w, h);
  
  XSync(state::get_dpy(), False);
  
  cydui::graphics::events::start();
  render::start(win);
  
  return win;
}

window_ti::window_ti(Window xwin, int w, int h) {
  //window_ti::window_ti(Window xwin, int w, int h) {
  this->xwin = xwin;
  this->staging_target = new cydui::graphics::render_target_t(this, w, h);
  this->render_target = new cydui::graphics::render_target_t(this, w, h);
}

render_target_ti::render_target_ti(
  cydui::graphics::window_t* win, int w, int h
) {
  //std::lock_guard guard(win->x_mtx);
  this->drawable = XCreatePixmap(state::get_dpy(),
    win->xwin,
    w,
    h,
    DefaultDepth(state::get_dpy(), state::get_screen()));
  this->gc = XCreateGC(state::get_dpy(), this->drawable, 0, nullptr);
  XSetLineAttributes(
    state::get_dpy(), this->gc, 1, LineSolid, CapButt, JoinMiter);
  this->w = w;
  this->h = h;
  this->win = win;
  
  set_background();
}


void cydui::graphics::set_background(window_t* win) {
  XSetWindowBackground(state::get_dpy(),
    win->xwin,
    BlackPixel(state::get_dpy(), state::get_screen()));
  win->render_target->set_background();
  win->staging_target->set_background();
}

void cydui::graphics::render_target_t::set_background() {
  XSetBackground(state::get_dpy(),
    this->gc,
    BlackPixel(state::get_dpy(), state::get_screen()));
}

void cydui::graphics::resize(window_t* win, int w, int h) {
  if (w == 0 || h == 0) return;
  win->render_target->resize(w, h);
  win->staging_target->resize(w, h);
}

void cydui::graphics::render_target_t::resize(int nw, int nh) {
  if (w == 0 || h == 0) return;
  if (nw != this->w || nh != this->h) {
    render::resize(this, nw, nh);
  }
}

void cydui::graphics::flush(window_t* win) {
  render::flush(win);
}

void cydui::graphics::clr_rect(
  render_target_t* target, int x, int y, unsigned int w, unsigned int h
) {
  render::clr_rect(target, x, y, w, h);
}

void cydui::graphics::drw_line(
  render_target_t* target,
  color::Color color,
  int x,
  int y,
  int x1,
  int y1
) {
  render::drw_line(target, color, x, y, x1, y1);
}

void cydui::graphics::drw_rect(
  render_target_t* target,
  color::Color color,
  int x,
  int y,
  int w,
  int h,
  bool filled
) {
  render::drw_rect(target, color, x, y, w, h, filled);
}

void cydui::graphics::drw_arc(
  render_target_t* target,
  color::Color color,
  int x,
  int y,
  int w,
  int h,
  int a0,
  int a1,
  bool filled
) {
  render::drw_arc(target, color, x, y, w, h, a0, a1, filled);
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
  cydui::graphics::window_t* win, font::Font* font
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
  cydui::graphics::window_t* win, font::Font* font
) {
  // TODO - Implement
}

static window_image load_image(
  cydui::graphics::window_t* win, cydui::layout::images::image_t* img
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
  cydui::graphics::render_target_t* target,
  cydui::layout::images::image_t* img
) {
  // TODO - Implement
}

void cydui::graphics::drw_text(
  render_target_t* target,
  font::Font* font,
  color::Color color,
  str text,
  int x,
  int y
) {
  window_font xfont = load_font(target->win, font);
  render::drw_text(target, xfont, color, text, x, y);
}

static std::unordered_map<str, XftFont*> cached_fonts;

std::pair<int, int> cydui::graphics::get_text_size(
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
  return {x_glyph_info.width + x_glyph_info.xOff, x_glyph_info.y};
}

void cydui::graphics::drw_image(
  render_target_t* target,
  layout::images::image_t* img,
  int x,
  int y,
  int w,
  int h
) {
  window_image i = load_image(target->win, img);
  render::drw_image(target, i, x, y, w, h);
}

std::pair<int, int> cydui::graphics::get_image_size(
  layout::images::image_t* img
) {
  imgs::img_data data = imgs::read_jpg(img->path);
  return {data.width, data.height};
}

void cydui::graphics::drw_target(
  render_target_t* dest_target,
  render_target_t* source_target,
  int xs,
  int ys,
  int xd,
  int yd,
  int w,
  int h
) {
  render::drw_target(dest_target, source_target, xs, ys, xd, yd, w, h);
}