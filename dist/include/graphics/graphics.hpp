//
// Created by castle on 8/21/22.
//

#ifndef CYD_UI_GRAPHICS_HPP
#define CYD_UI_GRAPHICS_HPP


#include "color.h"
#include "font.h"
#include "images.h"
#include "pixelmap.h"

namespace cydui::graphics {
    struct window_t;
    
    window_t* create_window(
      const char* title,
      const char* wclass,
      int x,
      int y,
      int w,
      int h,
      bool override_redirect = false
    );
    
    void resize(window_t* win, int w, int h);
    
    pixelmap_t* get_frame(window_t* win);
    
    void flush(window_t* win);
    
    unsigned long get_id(window_t* win);
}// namespace cydui::graphics

#include "compositing.h"

#endif//CYD_UI_GRAPHICS_HPP
