//
// Created by castle on 8/21/22.
//

#include "../../../include/graphics.hpp"
#include "x11_impl.hpp"
#include "../../../include/logging.hpp"
#include "../events.hpp"
#include "render/render.hpp"
#include "state/state.hpp"
#include <X11/Xft/Xft.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

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
    case 0:return NorthWestGravity;
    case XNegative:return NorthEastGravity;
    case YNegative:return SouthWestGravity;
  }
  
  return SouthEastGravity;
}

cydui::graphics::window_t* cydui::graphics::create_window(
  const char* title, const char* wclass, int x, int y, int w, int h
) {
  static int _ig = XInitThreads();
  
  XSetWindowAttributes wa         = {
    .background_pixmap =
    BlackPixel(state::get_dpy(), state::get_screen()),//ParentRelative,
    .bit_gravity = NorthWestGravity,
    .event_mask  = FocusChangeMask | KeyPressMask | KeyReleaseMask
      | VisibilityChangeMask | StructureNotifyMask | ButtonMotionMask
      | ButtonPressMask | ButtonReleaseMask | ExposureMask
      | PointerMotionMask,
    //.override_redirect = true // This makes it immutable across workspaces
  };
  std::string          title_str  = title;
  std::string          wclass_str = wclass;
  XClassHint           ch         = {
    title_str.data(),
    wclass_str.data()
  };
  
  int          x_o, y_o;
  unsigned int w_o, h_o;
  std::string  geom;
  geom += "0x0";
  if (x >= 0)
    geom += "+";
  geom += std::to_string(x);
  if (y >= 0)
    geom += "+";
  geom += std::to_string(y);
  
  int gm_mask = XParseGeometry(geom.c_str(), &x_o, &y_o, &w_o, &h_o);
  
  if (gm_mask & XNegative)
    x_o += DisplayWidth(state::get_dpy(), state::get_screen()) - w; // - 2;
  if (gm_mask & YNegative)
    y_o += DisplayHeight(state::get_dpy(), state::get_screen()) - h; // - 2;
  
  Window xwin = XCreateWindow(
    state::get_dpy(),
    state::get_root(),
    x_o,
    y_o,
    w,
    h,
    0,
    CopyFromParent,
    InputOutput,
    CopyFromParent,
    CWBorderPixel | CWBitGravity | CWColormap | CWBackPixmap | CWEventMask,
    &wa
  );
  XSetClassHint(state::get_dpy(), xwin, &ch);
  XStoreName(state::get_dpy(), xwin, title);
  XSync(state::get_dpy(), False);
  
  log_task.info("Created window %X at (%s) x: %d, y: %d", xwin, geom.c_str(), x, y);
  
  XWMHints wm = {.flags = InputHint, .input = 1};
  XSizeHints* sizeh;
  sizeh = XAllocSizeHints();
  sizeh->flags      = PSize | PResizeInc | PBaseSize | PMinSize;
  sizeh->height     = h;
  sizeh->width      = w;
  sizeh->height_inc = 1;
  sizeh->width_inc  = 1;
  //sizeh->base_height = 2 * borderpx;
  ////  sizeh->base_width = 2 * borderpx;
  ////  sizeh->min_height = win.ch + 2 * borderpx;
  ////  sizeh->min_width = win.cw + 2 * borderpx;
  ////  if (xw.isfixed) {
  ////    sizeh->flags |= PMaxSize;
  ////    sizeh->min_width = sizeh->max_width = w;
  ////    sizeh->min_height = sizeh->max_height = h;
  ////  }
  if (gm_mask & (XValue | YValue)) {
    sizeh->flags |= USPosition | PWinGravity;
    sizeh->x           = x_o;
    sizeh->y           = y_o;
    sizeh->win_gravity = geom_mask_to_gravity(gm_mask);
  }
  //
  XSetWMProperties(
    state::get_dpy(), xwin, NULL, NULL, NULL, 0, sizeh, &wm,
    &ch
  );
  XSync(state::get_dpy(), False);
  ////  r = XSetClassHint(state::get_dpy(), xwin, &ch);
  //  log_task.info("set hints");
  
  //  XDefineCursor(state::get_dpy(), xwin, state::cursor[CurNormal]->cursor);
  //if (x_o != 0 || y_o != 0) {
  //  log_task.info("Mapping RAISED window %X", xwin);
  //  XMapRaised(state::get_dpy(), xwin);
  //} else {
  log_task.info("Mapping window %X", xwin);
  XMapWindow(state::get_dpy(), xwin);
  //}
  
  XSync(state::get_dpy(), False);
  
  auto* win = new window_t { };
  win->xwin     = xwin;
  win->drawable = XCreatePixmap(
    state::get_dpy(),
    xwin,
    w,
    h,
    DefaultDepth(state::get_dpy(), state::get_screen()));
  win->gc       = XCreateGC(state::get_dpy(), win->drawable, 0, NULL);
  
  win->staging_drawable = XCreatePixmap(
    state::get_dpy(),
    xwin,
    w,
    h,
    DefaultDepth(state::get_dpy(), state::get_screen()));
  
  XSetLineAttributes(
    state::get_dpy(), win->gc, 1, LineSolid, CapButt, JoinMiter
  );
  
  XSync(state::get_dpy(), False);
  
  cydui::graphics::events::start();
  render::start(win);
  
  XFree(sizeh);
  return win;
}


void cydui::graphics::set_background(window_t* win) {
  XSetWindowBackground(
    state::get_dpy(),
    win->xwin,
    BlackPixel(state::get_dpy(), state::get_screen()));
  XSetBackground(
    state::get_dpy(),
    win->gc,
    BlackPixel(state::get_dpy(), state::get_screen()));
}

void cydui::graphics::resize(window_t* win, int w, int h) {
  if (w != win->w || h != win->h) {
    render::resize(win, w, h);
  }
}

void cydui::graphics::flush(window_t* win) {
  render::flush(win);
}

void cydui::graphics::clr_rect(
  window_t* win, int x, int y, unsigned int w, unsigned int h
) {
  render::clr_rect(win, x, y, w, h);
}

void cydui::graphics::drw_line(
  window_t* win,
  cydui::layout::color::Color* color,
  int x,
  int y,
  int x1,
  int y1
) {
  render::drw_line(win, color, x, y, x1, y1);
}

void cydui::graphics::drw_rect(
  window_t* win,
  cydui::layout::color::Color* color,
  int x,
  int y,
  int w,
  int h,
  bool filled
) {
  render::drw_rect(win, color, x, y, w, h, filled);
}

void cydui::graphics::drw_arc(
  window_t* win,
  cydui::layout::color::Color* color,
  int x,
  int y,
  int w,
  int h,
  int a0,
  int a1,
  bool filled
) {
  render::drw_arc(win, color, x, y, w, h, a0, a1, filled);
}

static std::string to_pattern(cydui::layout::fonts::Font* font) {
  std::string str;
  str.append(font->name + ":");
  str.append("size=" + std::to_string(font->size) + ":");
  str.append("antialias=");
  str.append((font->antialias? "true" : "false"));
  str.append(":");
  str.append("autohint=");
  str.append((font->autohint? "true" : "false"));
  //str.append(":");
  
  return str;
}

static window_font load_font(
  cydui::graphics::window_t* win, cydui::layout::fonts::Font* font
) {
  std::string font_spec = to_pattern(font);
  if (win->loaded_fonts.contains(font_spec))
    return win->loaded_fonts[font_spec];
  XftFont  * xfont;
  FcPattern* pattern;
  
  if (!(xfont   = XftFontOpenName(
    state::get_dpy(), state::get_screen(), font_spec.c_str()))) {
    log_task.error("Cannot load font from name %s", font_spec.c_str());
    return { };
  }
  if (!(pattern = FcNameParse((FcChar8*)(font_spec.c_str())))) {
    log_task.error("Cannot parse font name to pattern: %s", font_spec.c_str());
    return { };
  }
  
  auto f = window_font {.xfont = xfont, .pattern = pattern};
  win->loaded_fonts[font_spec] = f;
  return f;
}

static void unload_font(
  cydui::graphics::window_t* win, cydui::layout::fonts::Font* font
) {
  // TODO - Implement
}

void cydui::graphics::drw_text(
  window_t* win,
  layout::fonts::Font* font,
  layout::color::Color* color,
  std::string text,
  int x,
  int y
) {
  window_font xfont = load_font(win, font);
  render::drw_text(win, xfont, color, text, x, y);
}
