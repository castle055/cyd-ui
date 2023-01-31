//
// Created by castle on 8/21/22.
//

#ifndef CYD_UI_GRAPHICS_HPP
#define CYD_UI_GRAPHICS_HPP


#include "events.hpp"
#include "colors.hpp"
#include "fonts.hpp"

#include "x11_impl.hpp"
#include "images.h"

namespace cydui::graphics {
    typedef window_t window_t;
    
    window_t* create_window(
      const char* title, const char* wclass, int x, int y, int w, int h,
      bool override_redirect = false
    );
    
    void set_background(window_t* win);
    
    void resize(window_t* win, int w, int h);
    
    void flush(window_t* win);
    
    void clr_rect(window_t* win, int x, int y, unsigned int w, unsigned int h);
    
    void drw_line(
      window_t* win, layout::color::Color* color, int x, int y, int x1, int y1
    );
    
    void drw_rect(
      window_t* win,
      layout::color::Color* color,
      int x,
      int y,
      int w,
      int h,
      bool filled
    );
    
    void drw_arc(
      window_t* win,
      layout::color::Color* color,
      int x,
      int y,
      int w,
      int h,
      int a0,
      int a1,
      bool filled
    );
    
    void drw_text(
      window_t* win,
      layout::fonts::Font* font,
      layout::color::Color* color,
      std::string text,
      int x,
      int y
    );
    
    std::pair<int, int> get_text_size(
      layout::fonts::Font* font,
      const std::string &text
    );
    
    void drw_image(
      window_t* win,
      layout::images::image_t* img,
      int x,
      int y,
      int w,
      int h
    );
    
    std::pair<int, int> get_image_size(
      layout::images::image_t* img
    );
  
}// namespace cydui::graphics

#endif//CYD_UI_GRAPHICS_HPP
