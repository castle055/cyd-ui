//
// Created by castle on 8/23/22.
//

#ifndef CYD_UI_X11_IMPL_H
#define CYD_UI_X11_IMPL_H

#include "../../threading/threading.h"
#include "../graphics.h"

#include <deque>
#include <mutex>

struct window_render_req {
  int x, y, w, h;
};

struct cydui::graphics::window_t {
  Window xwin;
  Drawable drawable;
  GC gc;
  int w;
  int h;
  std::deque<window_render_req> render_reqs;
  std::mutex render_mtx;
  std::mutex x_mtx;
  threading::thread_t* render_thd;
};

#endif//CYD_UI_X11_IMPL_H
