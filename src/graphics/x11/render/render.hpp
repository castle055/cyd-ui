//
// Created by castle on 8/23/22.
//

#ifndef CYD_UI_RENDER_HPP
#define CYD_UI_RENDER_HPP

#include "../../../../include/graphics.hpp"
#include "../../../../include/x11_impl.hpp"

namespace render {
    struct RenderThreadData {
      cydui::graphics::window_t* win;
    };
    
    void start(cydui::graphics::window_t* win);
    
    void resize(cydui::graphics::window_t* win, int w, int h);
    
    void flush(cydui::graphics::window_t* win);
    
    void clr_rect(
      cydui::graphics::window_t* win,
      int x,
      int y,
      unsigned int w,
      unsigned int h
    );
    
    void drw_line(
      cydui::graphics::window_t* win,
      cydui::layout::color::Color* color,
      int x,
      int y,
      int x1,
      int y1
    );
    
    void drw_rect(
      cydui::graphics::window_t* win,
      cydui::layout::color::Color* color,
      int x,
      int y,
      int w,
      int h,
      bool filled
    );
    
    void drw_arc(
      cydui::graphics::window_t* win,
      cydui::layout::color::Color* color,
      int x,
      int y,
      int w,
      int h,
      int a0,
      int a1,
      bool filled
    );
    
    void drw_text(
      cydui::graphics::window_t* win,
      window_font font,
      cydui::layout::color::Color* color,
      const std::string &text,
      int x,
      int y
    );
    
    void drw_image(
      cydui::graphics::window_t* win,
      window_image img,
      int x,
      int y,
      int w,
      int h
    );
  
}// namespace render


#endif//CYD_UI_RENDER_HPP
