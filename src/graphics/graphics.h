//
// Created by castle on 8/21/22.
//

#ifndef CYD_UI_GRAPHICS_H
#define CYD_UI_GRAPHICS_H


#include "../events/events.h"
#include "../layout/color/colors.h"
#include "../layout/fonts/fonts.h"
#include <X11/Xlib.h>

namespace cydui::graphics {
  struct window_t;

  window_t* create_window(
      char* title, char* wclass, int x, int y, int w, int h);

  void set_background(window_t* win);

  void on_event(window_t* win, events::graphics::CGraphicsEvent* ev);

  void clr_rect(window_t* win, int x, int y, unsigned int w, unsigned int h);

  void drw_line(
      window_t* win, layout::color::Color* color, int x, int y, int x1, int y1);

  void drw_rect(window_t* win,
      layout::color::Color* color,
      int x,
      int y,
      int w,
      int h,
      bool filled);

  void drw_arc(window_t* win,
      layout::color::Color* color,
      int x,
      int y,
      int w,
      int h,
      int a0,
      int a1,
      bool filled);

  void draw_text(window_t* win,
      layout::fonts::Font* font,
      layout::color::Color* color,
      std::string text);
}// namespace cydui::graphics

#endif//CYD_UI_GRAPHICS_H
