//
// Created by castle on 8/22/22.
//

#ifndef CYD_UI_WINDOW_TYPES_HPP
#define CYD_UI_WINDOW_TYPES_HPP

#include "events.hpp"
#include "graphics.hpp"

namespace cydui::layout {
  class Layout;
}
namespace cydui::window {
  class CWindow;
  
  class CWindow {
  public:
    graphics::window_t* win_ref;
    layout::Layout    * layout;
  };
  
}// namespace cydui::window
#endif//CYD_UI_WINDOW_TYPES_HPP
