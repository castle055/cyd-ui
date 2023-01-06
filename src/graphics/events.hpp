//
// Created by castle on 8/21/22.
//

#ifndef CYD_UI_X11_EVENTS_H
#define CYD_UI_X11_EVENTS_H

#include "../../include/change_events.h"

namespace cydui::graphics::events {
  void start();
  
}

EVENT(RedrawEvent, {
  unsigned int win = 0;
  int          x   = 0;
  int          y   = 0;
  void* component = nullptr;
})
EVENT(KeyEvent, {
  const unsigned int win      = 0;
  const unsigned int key      = 0;
  const bool         pressed  = false;
  const bool         released = false;
  const bool         holding  = false;
})
EVENT(ButtonEvent, {
  const unsigned int win      = 0;
  const unsigned int button   = 0;
  const int          x        = 0;
  const int          y        = 0;
  const bool         pressed  = false;
  const bool         released = false;
  const bool         holding  = false;
})
EVENT(ScrollEvent, {
  unsigned int win = 0;
  int          d   = 0;
  int          x   = 0;
  int          y   = 0;
})
EVENT(MotionEvent, {
  unsigned int win   = 0;
  int          x     = 0;
  int          y     = 0;
  bool         enter = false;
  bool         exit  = false;
})
EVENT(ResizeEvent, {
  unsigned int win = 0;
  int          w   = 0;
  int          h   = 0;
})


#endif//CYD_UI_X11_EVENTS_H
