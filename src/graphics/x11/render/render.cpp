//
// Created by castle on 8/23/22.
//

#include "render.h"
#include "../../../logging/logging.h"
#include "../state/state.h"
#include "../x11_impl.h"

#include <X11/Xlib.h>

logging::logger xlog_ctrl = {.name = "X11::RENDER::CTRL", .on = false};
logging::logger xlog_task = {.name = "X11::RENDER::TASK", .on = false};

void render_sbr(cydui::graphics::window_t* win) {
  int minx = 0;
  int miny = 0;
  int maxx = 0;
  int maxy = 0;
  window_render_req req;
  win->render_mtx.lock();
  while (!win->render_reqs.empty()) {
    req = win->render_reqs.front();
    win->render_reqs.pop_front();
    if (req.x < minx)
      minx = req.x;
    if (req.y < miny)
      miny = req.y;
    if (req.x + req.w > maxx)
      maxx = req.x + req.w;
    if (req.y + req.h > maxy)
      maxy = req.y + req.h;
    //    xlog_task.debug("Rendering area, pos(x=%d,y=%d) size(w=%d,h=%d)", req.x, req.y, req.w, req.h);
    //    XClearArea(state::get_dpy(), win->xwin, req.x, req.y, req.w, req.h, False);
  }
  win->render_mtx.unlock();

  win->x_mtx.lock();
  XCopyArea(state::get_dpy(),
      win->drawable,
      win->xwin,
      win->gc,
      minx,
      miny,
      maxx - minx,
      maxy - miny,
      minx,
      miny);
  win->x_mtx.unlock();
  XSync(state::get_dpy(), False);
}

void render_task(cydui::threading::thread_t* this_thread) {
  xlog_task.debug("Started render thread");
  auto* render_data = (render::RenderThreadData*)this_thread->data;
  while (this_thread->running) {
    render_sbr(render_data->win);
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
  window_render_req req = {
      .x = x,
      .y = y,
      .w = w,
      .h = h,
  };
  xlog_ctrl.debug("REQ Render area, pos(x=%d,y=%d) size(w=%d,h=%d)",
      req.x,
      req.y,
      req.w,
      req.h);
  win->render_mtx.lock();
  win->render_reqs.push_back(req);
  win->render_mtx.unlock();
}

unsigned long color_to_xcolor(cydui::layout::color::Color* color) {
  Colormap map = DefaultColormap(state::get_dpy(), state::get_screen());
  XColor c;
  XParseColor(state::get_dpy(), map, color->to_string().c_str(), &c);
  XAllocColor(state::get_dpy(), map, &c);

  unsigned long cp = c.pixel;

  return cp;
}

void render::resize(cydui::graphics::window_t* win, int w, int h) {
  if (w > win->w || h > win->h) {
    win->x_mtx.lock();
    XFreePixmap(state::get_dpy(), win->drawable);
    win->drawable = XCreatePixmap(state::get_dpy(),
        win->xwin,
        w,
        h,
        DefaultDepth(state::get_dpy(), state::get_screen()));
    XFreeGC(state::get_dpy(), win->gc);
    win->gc = XCreateGC(state::get_dpy(), win->drawable, 0, NULL);
    win->x_mtx.unlock();
    XSync(state::get_dpy(), False);
  }
  win->w = w;
  win->h = h;
}

void render::clr_rect(cydui::graphics::window_t* win,
    int x,
    int y,
    unsigned int w,
    unsigned int h) {
  XSetForeground(state::get_dpy(),
      win->gc,
      BlackPixel(state::get_dpy(), state::get_screen()));
  XFillRectangle(state::get_dpy(), win->drawable, win->gc, x, y, w, h);
  req(win, x, y, (int)w + 1, (int)h + 1);// added 1 margin for lines
}

void render::drw_line(cydui::graphics::window_t* win,
    cydui::layout::color::Color* color,
    int x,
    int y,
    int x1,
    int y1) {
  if (color) {
    xlog_ctrl.error("color: %s", color->to_string().c_str());
    XSetForeground(state::get_dpy(), win->gc, color_to_xcolor(color));
  }
  XSetBackground(state::get_dpy(),
      win->gc,
      BlackPixel(state::get_dpy(), state::get_screen()));
  XDrawLine(state::get_dpy(), win->drawable, win->gc, x, y, x1, y1);
  req(win, x, y, x1 - x + 1, y1 - y + 1);// added 1 margin for lines
}

void render::drw_rect(cydui::graphics::window_t* win,
    cydui::layout::color::Color* color,
    int x,
    int y,
    int w,
    int h,
    bool filled) {
  if (color) {
    xlog_ctrl.error("color: %s", color->to_string().c_str());
    XSetForeground(state::get_dpy(), win->gc, color_to_xcolor(color));
  }
  XSetBackground(state::get_dpy(),
      win->gc,
      BlackPixel(state::get_dpy(), state::get_screen()));
  if (filled) {
    XFillRectangle(state::get_dpy(), win->drawable, win->gc, x, y, w, h);
  } else {
    XDrawRectangle(state::get_dpy(), win->drawable, win->gc, x, y, w, h);
  }
  req(win, x, y, w + 1, h + 1);// added 1 margin for lines
}

void render::drw_arc(cydui::graphics::window_t* win,
    cydui::layout::color::Color* color,
    int x,
    int y,
    int w,
    int h,
    int a0,
    int a1,
    bool filled) {
  win->x_mtx.lock();
  if (color) {
    xlog_ctrl.error("color: %s", color->to_string().c_str());
    XSetForeground(state::get_dpy(), win->gc, color_to_xcolor(color));
  }
  XSetBackground(state::get_dpy(),
      win->gc,
      BlackPixel(state::get_dpy(), state::get_screen()));
  if (filled) {
    XFillArc(state::get_dpy(), win->drawable, win->gc, x, y, w, h, a0, a1);
  } else {
    XDrawArc(state::get_dpy(), win->drawable, win->gc, x, y, w, h, a0, a1);
  }
  win->x_mtx.unlock();
  req(win, x, y, w + 1, h + 1);// added 1 margin for lines
}
