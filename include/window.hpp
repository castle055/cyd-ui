//
// Created by castle on 8/21/22.
//

#ifndef CYD_UI_WINDOW_HPP
#define CYD_UI_WINDOW_HPP


#include "../src/events/events.hpp"
#include "../src/graphics/graphics.hpp"
#include "layout.hpp"
#include "window_types.hpp"

namespace cydui::window {
  CWindow* create(
    layout::Layout* layout,
    const char* title = "CydUI",
    const char* wclass = "cydui",
    int x = 0,
    int y = 0,
    int w = 640,
    int h = 480
  );
  
  bool process_event(CWindow win);
}// namespace cydui::window


#endif//CYD_UI_WINDOW_HPP
