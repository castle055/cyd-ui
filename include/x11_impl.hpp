//
// Created by castle on 8/23/22.
//

#ifndef CYD_UI_X11_IMPL_HPP
#define CYD_UI_X11_IMPL_HPP

#include "threading.hpp"

#include <X11/Xft/Xft.h>
#include <deque>
#include <mutex>
#include <unordered_map>

struct window_render_req {
  int x, y, w, h;
};

struct window_font {
  XftFont  * xfont;
  FcPattern* pattern;
};
typedef std::unordered_map<std::string, window_font> loaded_font_map_t;

struct window_t {
  Window                        xwin;
  Drawable                      drawable;
  Drawable                      staging_drawable;
  int                           staging_w;
  int                           staging_h;
  GC                            gc;
  int                           w;
  int                           h;
  std::deque<window_render_req> render_reqs;
  bool                          dirty     = true;
  std::mutex                    render_mtx;
  std::mutex                    x_mtx;
  cydui::threading::thread_t* render_thd;
  void                      * render_data = nullptr;
  loaded_font_map_t loaded_fonts;
};

#endif//CYD_UI_X11_IMPL_HPP
