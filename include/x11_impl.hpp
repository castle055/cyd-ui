//
// Created by castle on 8/23/22.
//

#ifndef CYD_UI_X11_IMPL_HPP
#define CYD_UI_X11_IMPL_HPP

#include "threading.hpp"
#include "color.h"

#include <X11/Xft/Xft.h>
#include <deque>
#include <mutex>
#include <unordered_map>
#include <vector>

struct window_render_req {
  int x, y, w, h;
};

struct window_font {
  XftFont* xfont;
  FcPattern* pattern;
};
typedef std::unordered_map<str, window_font> loaded_font_map_t;

struct window_image {
  XImage* ximg;
};
typedef std::unordered_map<str, window_image> loaded_images_map_t;


struct window_ti;

struct render_target_ti {
  Drawable drawable;
  GC gc;
  int w;
  int h;
  
  window_ti* win;
  
  render_target_ti(window_ti* win, int w, int h);
  
  void set_background();
  
  void resize(int w, int h);
};


enum class render_req_type_e {
  LINE,
  RECTANGLE,
  ARC,
  TEXT,
  IMAGE,
  TARGET,
  FLUSH,
};

struct render_req_t {
  render_req_type_e type = render_req_type_e::LINE;
  render_target_ti* target = nullptr;
  color::Color color {};
  bool filled = false;
  window_font font {};
  std::string text {};
  window_image image {};
  render_target_ti* source_target = nullptr;
  int x = 0;
  int y = 0;
  int w = 0;
  int h = 0;
  int a0_xs = 0;
  int a1_ys = 0;
};


struct window_ti {
  char padd[128];
  Window xwin;
  render_target_ti* staging_target = nullptr;
  render_target_ti* render_target = nullptr;
  //Drawable drawable;
  //Drawable staging_drawable;
  //int staging_w;
  //int staging_h;
  //GC gc;
  //int w;
  //int h;
  std::deque<render_req_t> render_reqs;
  bool dirty = true;
  std::mutex render_mtx;
  std::mutex x_mtx;
  loaded_font_map_t loaded_fonts;
  loaded_images_map_t loaded_images;
  
  window_ti(Window xwin, int w, int h);
  
  cydui::threading::thread_t* render_thd = nullptr;
  void* render_data = nullptr;
};

#endif//CYD_UI_X11_IMPL_HPP
