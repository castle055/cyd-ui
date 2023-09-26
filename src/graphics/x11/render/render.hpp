//
// Created by castle on 8/23/22.
//

#ifndef CYD_UI_RENDER_HPP
#define CYD_UI_RENDER_HPP

namespace render {
    struct RenderThreadData {
      cydui::graphics::window_t* win;
    };
    
    void start(cydui::graphics::window_t* win);
    
    void resize(cydui::graphics::render_target_t* target, int w, int h);
    
    void flush(cydui::graphics::window_t* win);
    
    void clr_rect(
      cydui::graphics::render_target_t* target,
      int x,
      int y,
      unsigned int w,
      unsigned int h
    );
    
    void drw_line(
      cydui::graphics::render_target_t* target,
      color::Color color,
      int x,
      int y,
      int x1,
      int y1
    );
    
    void drw_rect(
      cydui::graphics::render_target_t* target,
      color::Color color,
      int x,
      int y,
      int w,
      int h,
      bool filled
    );
    
    void drw_arc(
      cydui::graphics::render_target_t* target,
      color::Color color,
      int x,
      int y,
      int w,
      int h,
      int a0,
      int a1,
      bool filled
    );
    
    void drw_text(
      cydui::graphics::render_target_t* target,
      window_font font,
      color::Color color,
      const str &text,
      int x,
      int y
    );
    
    void drw_image(
      cydui::graphics::render_target_t* target,
      window_image img,
      int x,
      int y,
      int w,
      int h
    );
    
    void drw_target(
      cydui::graphics::render_target_t* dest_target,
      cydui::graphics::render_target_t* source_target,
      int xs,
      int ys,
      int xd,
      int yd,
      int w,
      int h
    );
  
}// namespace render


#endif//CYD_UI_RENDER_HPP
