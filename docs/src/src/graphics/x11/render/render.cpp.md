//
// Created by castle on 8/23/22.
//

#include "render.hpp"
#include "../../../logging/logging.hpp"
#include "../state/state.hpp"
#include "../x11_impl.hpp"

#include <X11/Xlib.h>

logging::logger xlog_ctrl = {.name = "X11::RENDER::CTRL", .on = false};
logging::logger xlog_task = {.name = "X11::RENDER::TASK", .on = true};

void render_sbr(cydui::graphics::window_t* win) {
  window_render_req req;
  
  if (!win->render_mtx.try_lock())
    return;
  bool dirty = win->dirty;
  win->dirty = false;
  win->render_mtx.unlock();
  
  if (!dirty)
    return;
  
  //win->x_mtx.lock();
  XCopyArea(
      state::get_dpy(),
      win->drawable,
      win->xwin,
      win->gc,
      0, 0,
      win->w, win->h,
      0, 0
  );
  //win->x_mtx.unlock();
  
  XFlush(state::get_dpy());
}

using namespace std::chrono_literals;

void render_task(cydui::threading::thread_t* this_thread) {
  xlog_task.debug("Started render thread");
  auto* render_data = (render::RenderThreadData*)this_thread->data;
  while (this_thread->running) {
    render_sbr(render_data->win);
    std::this_thread::sleep_for(30ms);
  }
}

static render::RenderThreadData* render_data = nullptr;

void render::start(cydui::graphics::window_t* win) {
  if (win->render_thd && win->render_thd->running)
    return;
  xlog_ctrl.debug("Starting render thread");
  
  delete render_data;
  render_data = new RenderThreadData {
      .win = win,
  };
  win->render_thd = cydui::threading::new_thread(render_task, render_data);
}

static void req(cydui::graphics::window_t* win, int x, int y, int w, int h) {
  //window_render_req req = {
  //    .x = x,
  //    .y = y,
  //    .w = w,
  //    .h = h,
  //};
  //xlog_ctrl.debug(
  //    "REQ Render area, pos(x=%d,y=%d) size(w=%d,h=%d)",
  //    req.x,
  //    req.y,
  //    req.w,
  //    req.h
  //);
  win->render_mtx.lock();
  //win->render_reqs.push_back(req);
  win->dirty = true;
  win->render_mtx.unlock();
}

XColor color_to_xcolor(cydui::layout::color::Color* color) {
  Colormap map = DefaultColormap(state::get_dpy(), state::get_screen());
  XColor   c;
  XParseColor(state::get_dpy(), map, color->to_string().c_str(), &c);
  XAllocColor(state::get_dpy(), map, &c);
  
  return c;
}

XftColor* color_to_xftcolor(cydui::layout::color::Color* color) {
  auto* c = new XftColor;
  
  if (!XftColorAllocName(
      state::get_dpy(),
      DefaultVisual(state::get_dpy(), state::get_screen()),
      DefaultColormap(state::get_dpy(), state::get_screen()),
      color->to_string().c_str(),
      c
  )) {
    xlog_ctrl.error("Cannot allocate color %s", color->to_string().c_str());
  }
  
  return c;
}

void render::resize(cydui::graphics::window_t* win, int w, int h) {
  if (w > win->w || h > win->h) {
    
    win->x_mtx.lock();
    
    Drawable new_drw = XCreatePixmap(
        state::get_dpy(),
        win->xwin,
        w,
        h,
        DefaultDepth(state::get_dpy(), state::get_screen()));
    XCopyArea(state::get_dpy(), win->drawable, new_drw, win->gc, 0, 0, win->w, win->h, 0, 0);
    XFlush(state::get_dpy());
    
    XFreePixmap(state::get_dpy(), win->drawable);
    win->drawable = new_drw;
    
    XFreeGC(state::get_dpy(), win->gc);
    win->gc = XCreateGC(state::get_dpy(), win->drawable, 0, NULL);
    
    XFlush(state::get_dpy());
    
    win->x_mtx.unlock();
    req(win, 0, 0, 0, 0);
  }
  win->w = w;
  win->h = h;
}

void render::clr_rect(
    cydui::graphics::window_t* win,
    int x,
    int y,
    unsigned int w,
    unsigned int h
) {
  win->x_mtx.lock();
  
  XSetForeground(
      state::get_dpy(),
      win->gc,
      BlackPixel(state::get_dpy(), state::get_screen()));
  XFillRectangle(state::get_dpy(), win->drawable, win->gc, x, y, w, h);
  
  win->x_mtx.unlock();
  
  //req(win, x, y, (int)w + 1, (int)h + 1);// added 1 margin for lines
}

void render::flush(
    cydui::graphics::window_t* win
) {
  req(win, 0, 0, 0, 0);
}

void render::drw_line(
    cydui::graphics::window_t* win,
    cydui::layout::color::Color* color,
    int x,
    int y,
    int x1,
    int y1
) {
  win->x_mtx.lock();
  
  if (color) {
    XSetForeground(state::get_dpy(), win->gc, color_to_xcolor(color).pixel);
  }
  XSetBackground(
      state::get_dpy(),
      win->gc,
      BlackPixel(state::get_dpy(), state::get_screen()));
  XDrawLine(state::get_dpy(), win->drawable, win->gc, x, y, x1, y1);
  
  win->x_mtx.unlock();
  
  //req(win, x, y, x1 - x + 1, y1 - y + 1);// added 1 margin for lines
}

void render::drw_rect(
    cydui::graphics::window_t* win,
    cydui::layout::color::Color* color,
    int x,
    int y,
    int w,
    int h,
    bool filled
) {
  win->x_mtx.lock();
  
  if (color) {
    XSetForeground(state::get_dpy(), win->gc, color_to_xcolor(color).pixel);
  }
  XSetBackground(
      state::get_dpy(),
      win->gc,
      BlackPixel(state::get_dpy(), state::get_screen()));
  if (filled) {
    XFillRectangle(state::get_dpy(), win->drawable, win->gc, x, y, w, h);
  } else {
    XDrawRectangle(state::get_dpy(), win->drawable, win->gc, x, y, w, h);
  }
  
  win->x_mtx.unlock();
  
  //req(win, x, y, w + 1, h + 1);// added 1 margin for lines
}

void render::drw_arc(
    cydui::graphics::window_t* win,
    cydui::layout::color::Color* color,
    int x,
    int y,
    int w,
    int h,
    int a0,
    int a1,
    bool filled
) {
  win->x_mtx.lock();
  if (color) {
    XSetForeground(state::get_dpy(), win->gc, color_to_xcolor(color).pixel);
  }
  XSetBackground(
      state::get_dpy(),
      win->gc,
      BlackPixel(state::get_dpy(), state::get_screen()));
  if (filled) {
    XFillArc(state::get_dpy(), win->drawable, win->gc, x, y, w, h, a0, a1);
  } else {
    XDrawArc(state::get_dpy(), win->drawable, win->gc, x, y, w, h, a0, a1);
  }
  win->x_mtx.unlock();
  //req(win, x, y, w + 1, h + 1);// added 1 margin for lines
}

void render::drw_text(
    cydui::graphics::window_t* win,
    window_font font,
    cydui::layout::color::Color* color,
    std::string text,
    int x,
    int y
) {
  if (!color)
    color = new cydui::layout::color::Color("#FCAE1E");
  
  XColor c = color_to_xcolor(color);
  XftColor* xft_c    = color_to_xftcolor(color);
  XftDraw * xft_draw = XftDrawCreate(
      state::get_dpy(), win->drawable, DefaultVisual(state::get_dpy(), state::get_screen()),
      DefaultColormap(state::get_dpy(), state::get_screen()));
  XGlyphInfo x_glyph_info;
  int        w, h;
  
  win->x_mtx.lock();
  
  XSetForeground(state::get_dpy(), win->gc, c.pixel);
  XSetBackground(
      state::get_dpy(),
      win->gc,
      BlackPixel(state::get_dpy(), state::get_screen()));
  
  XftDrawStringUtf8(
      xft_draw, xft_c, font.xfont, x, y, (FcChar8*)text.c_str(), text.size());
  XftTextExtentsUtf8(state::get_dpy(), font.xfont, (XftChar8*)text.c_str(), text.size(), &x_glyph_info);
  w = x_glyph_info.xOff;
  h = x_glyph_info.yOff;
  
  XftDrawDestroy(xft_draw);
  
  win->x_mtx.unlock();
  
  //req(win, x, y, w + 1, h + 1);// added 1 margin for lines
}
