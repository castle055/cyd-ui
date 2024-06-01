// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CYD_UI_X11_IMPL_HPP
#define CYD_UI_X11_IMPL_HPP

#include <X11/Xft/Xft.h>
#include <deque>
#include <mutex>
#include <unordered_map>
#include <vector>

#include "cyd_ui/threading.hpp"

using namespace cyd::ui::graphics;

struct window_font {
  XftFont* xfont;
  FcPattern* pattern;
};
typedef std::unordered_map<str, window_font> loaded_font_map_t;

struct window_image {
  XImage* ximg;
};
typedef std::unordered_map<str, window_image> loaded_images_map_t;

namespace cyd::ui::graphics {
    struct window_t {
      cyd::fabric::async::async_bus_t* bus;
      prof::context_t* profiler;
      Window xwin;
      GC gc;
      pixelmap_t* staging_target = nullptr;
      pixelmap_t* render_target = nullptr;
      //Drawable drawable;
      //Drawable staging_drawable;
      //int staging_w;
      //int staging_h;
      //GC gc;
      //int w;
      //int h;
      bool dirty = true;
      std::mutex render_mtx;
      loaded_font_map_t loaded_fonts;
      loaded_images_map_t loaded_images;
      
      window_t(
        cyd::fabric::async::async_bus_t* async_bus,
        prof::context_t* profiler,
        Window xwin,
        unsigned long w,
        unsigned long h
      );
      
      cyd::ui::threading::thread_t* render_thd = nullptr;
      void* render_data = nullptr;
    };
}

#endif//CYD_UI_X11_IMPL_HPP
