//
// Created by castle on 8/21/22.
//

#include "x11_impl.h"
#include "../events.h"
#include "state/state.h"
#include "../../logging/logging.h"
#include "render/render.h"
#include <X11/Xlib.h>
#include <X11/Xutil.h>

const logging::logger log_task = { .name = "X11_IMPL", .on = false};

char* log_error_code(int err) {
  static char buf[128];
  XGetErrorText(state::get_dpy(), err, buf, 128);
  return buf;
}

static Bool evpredicate() {
  return True;
}

cydui::graphics::window_t* cydui::graphics::create_window(
    char *title, char *wclass, int x, int y, int w, int h) {
  XInitThreads();
  XSetWindowAttributes wa = {
      .background_pixmap = BlackPixel(state::get_dpy(), state::get_screen()), //ParentRelative,
      .bit_gravity = NorthWestGravity,
      .event_mask = FocusChangeMask | KeyPressMask
          | KeyReleaseMask | VisibilityChangeMask
          | StructureNotifyMask | ButtonMotionMask
          | ButtonPressMask | ButtonReleaseMask
          | ExposureMask | PointerMotionMask,
//      .override_redirect = True // This makes it immutable across workspaces
  };
  XClassHint ch = {title, wclass};
  Window xwin = XCreateWindow(
      state::get_dpy(), state::get_root(), x, y,
      w, h, 0,
      CopyFromParent, CopyFromParent, CopyFromParent,
      CWBorderPixel | CWBitGravity | CWColormap |  CWBackPixmap | CWEventMask,
      &wa);
  XSetClassHint(state::get_dpy(), xwin, &ch);
  XStoreName(state::get_dpy(), xwin, title);
  XSync(state::get_dpy(), False);
  
  log_task.info("Created window %X", xwin);
  
//  XWMHints wm = {.flags = InputHint, .input = 1};
//  XSizeHints *sizeh;
//
//  sizeh = XAllocSizeHints();
//  sizeh->flags = PSize | PResizeInc | PBaseSize | PMinSize;
//  sizeh->height = h;
//  sizeh->width = w;
//  sizeh->height_inc = 1;
//  sizeh->width_inc = 1;
////  sizeh->base_height = 2 * borderpx;
////  sizeh->base_width = 2 * borderpx;
////  sizeh->min_height = win.ch + 2 * borderpx;
////  sizeh->min_width = win.cw + 2 * borderpx;
////  if (xw.isfixed) {
////    sizeh->flags |= PMaxSize;
////    sizeh->min_width = sizeh->max_width = w;
////    sizeh->min_height = sizeh->max_height = h;
////  }
////  if (xw.gm & (XValue|YValue)) {
////    sizeh->flags |= USPosition | PWinGravity;
////    sizeh->x = xw.l;
////    sizeh->y = xw.t;
////    sizeh->win_gravity = xgeommasktogravity(xw.gm);
////  }
//
//  XSetWMProperties(state::get_dpy(), xwin, NULL, NULL, NULL, 0, sizeh, &wm,
//                   &ch);
//  XFree(sizeh);
////  r = XSetClassHint(state::get_dpy(), xwin, &ch);
//  log_task.info("set hints");
  
//  XDefineCursor(state::get_dpy(), xwin, state::cursor[CurNormal]->cursor);
  log_task.info("Mapping window %X", xwin);
  XMapWindow(state::get_dpy(), xwin);
  
  XSync(state::get_dpy(), False);
  
  auto* win = new window_t { };
  win->xwin = xwin;
  win->drawable = XCreatePixmap(state::get_dpy(), xwin, w, h, DefaultDepth(state::get_dpy(), state::get_screen()));
  win->gc = XCreateGC(state::get_dpy(), win->drawable, 0, NULL);
  
  XSetLineAttributes(state::get_dpy(), win->gc, 1, LineSolid, CapButt, JoinMiter);
  
  XSync(state::get_dpy(), False);
  
  cydui::graphics::events::start();
  render::start(win);
  
  return win;
}


void cydui::graphics::set_background(window_t* win) {
  XSetWindowBackground(state::get_dpy(), win->xwin, BlackPixel(state::get_dpy(), state::get_screen()));
  XSetBackground(state::get_dpy(), win->gc, BlackPixel(state::get_dpy(), state::get_screen()));
}

void cydui::graphics::on_event(window_t* win, cydui::events::graphics::CGraphicsEvent *ev) {
  switch (ev->type) {
    case cydui::events::graphics::GPH_EV_RESIZE:
      if (ev->data.resize_ev.w != win->w
          || ev->data.resize_ev.h != win->h) {
        render::resize(
            win,
            ev->data.resize_ev.w,
            ev->data.resize_ev.h
        );
        cydui::events::emit(
            new cydui::events::CEvent {
                .type = cydui::events::EVENT_LAYOUT,
                .raw_event = nullptr,
                .data = new cydui::events::layout::CLayoutEvent {
                  .type = cydui::events::layout::LYT_EV_REDRAW,
                  .data = cydui::events::layout::CLayoutData {
                      .redraw_ev = cydui::events::layout::CRedrawEvent { .x = 0, .y = 0 }
                  }
                }
            });
        cydui::events::emit(
            new cydui::events::CEvent {
                .type = cydui::events::EVENT_LAYOUT,
                .raw_event = nullptr,
                .data = new cydui::events::layout::CLayoutEvent {
                    .type = cydui::events::layout::LYT_EV_RESIZE,
                    .data = cydui::events::layout::CLayoutData {
                        .resize_ev = cydui::events::layout::CResizeEvent {
                            .w = ev->data.resize_ev.w,
                            .h = ev->data.resize_ev.h
                        }
                    }
                }
            });
      }
      break;
    default:
      break;
  }
}

void cydui::graphics::clr_rect(window_t *win, int x, int y, unsigned int w, unsigned int h) {
  render::clr_rect(win, x, y, w, h);
}

void cydui::graphics::drw_line(window_t *win, cydui::layout::color::Color* color, int x, int y, int x1, int y1) {
  render::drw_line(win, color, x, y, x1, y1);
}

void cydui::graphics::drw_rect(window_t *win, cydui::layout::color::Color* color, int x, int y, int w, int h, bool filled) {
  render::drw_rect(win, color, x, y, w, h, filled);
}

void cydui::graphics::drw_arc(window_t *win, cydui::layout::color::Color* color, int x, int y, int w, int h, int a0, int a1, bool filled) {
  render::drw_arc(win, color, x, y, w, h, a0, a1, filled);
}

void cydui::graphics::draw_text(window_t *win, layout::fonts::Font *font, layout::color::Color *color,
                                std::string text) {
  
}
