//
// Created by castle on 8/21/22.
//

#ifndef CYD_UI_WINDOW_H
#define CYD_UI_WINDOW_H


#include "../src/graphics/graphics.h"
#include "../src/events/events.h"
#include "layout.h"
#include "window_types.h"

namespace cydui::window {
  CWindow* create(
      layout::Layout* layout,
      char* title = "CydUI",
      char* wclass = "cydui",
      int x = 0,
      int y = 0,
      int w = 640,
      int h = 480
  );
  
  bool process_event(CWindow win);
}


#endif //CYD_UI_WINDOW_H
