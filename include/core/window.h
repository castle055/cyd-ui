//
// Created by castle on 9/26/23.
//

#ifndef CYD_UI_WINDOW_H
#define CYD_UI_WINDOW_H

#include "../graphics/graphics.hpp"
#include "../cydstd/profiling.h"

namespace cydui::layout {
    class Layout;
}
namespace cydui::window {
    class CWindow {
    public:
      graphics::window_t* win_ref;
      layout::Layout* layout;
      
      prof::context_t profiling_ctx {};
      
      void terminate();
      
      std::pair<int, int> get_position() const;
      std::pair<int, int> get_size() const;
    };
    
    CWindow* create(
      layout::Layout* layout,
      const char* title = "CydUI",
      const char* wclass = "cydui",
      int x = 0,
      int y = 0,
      int w = 640,
      int h = 480,
      bool override_redirect = false
    );
}

#endif //CYD_UI_WINDOW_H
