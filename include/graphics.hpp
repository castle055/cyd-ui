//
// Created by castle on 8/21/22.
//

#ifndef CYD_UI_GRAPHICS_HPP
#define CYD_UI_GRAPHICS_HPP


#include "types.hpp"
#include "events.hpp"


namespace cydui::graphics {
    window_t* create_window(
      const char* title,
      const char* wclass,
      int x,
      int y,
      int w,
      int h,
      bool override_redirect = false
    );
    
    void set_background(window_t* win);
    
    void resize(window_t* win, int w, int h);
    
    void flush(window_t* win);
    
    void clr_rect(
      render_target_t* target, int x, int y, unsigned int w, unsigned int h
    );
    
    void drw_line(
      render_target_t* target,
      layout::color::Color* color,
      int x,
      int y,
      int x1,
      int y1
    );
    
    void drw_rect(
      render_target_t* target,
      layout::color::Color* color,
      int x,
      int y,
      int w,
      int h,
      bool filled
    );
    
    void drw_arc(
      render_target_t* target,
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
      render_target_t* target,
      layout::fonts::Font* font,
      layout::color::Color* color,
      str text,
      int x,
      int y
    );
    
    std::pair<int, int> get_text_size(
      layout::fonts::Font* font, const str &text
    );
    
    void drw_image(
      render_target_t* target,
      layout::images::image_t* img,
      int x,
      int y,
      int w,
      int h
    );
    
    std::pair<int, int> get_image_size(layout::images::image_t* img);
    
    void drw_target(
      render_target_t* dest_target,
      render_target_t* source_target,
      int xs,
      int ys,
      int xd,
      int yd,
      int w,
      int h
    );
  
}// namespace cydui::graphics

#endif//CYD_UI_GRAPHICS_HPP
