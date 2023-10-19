//
// Created by castle on 1/16/23.
//

#ifndef CYD_UI_CYDUI_EVENTS_H
#define CYD_UI_CYDUI_EVENTS_H

#include "change_events.h"

EVENT(RedrawEvent, {
  unsigned long win = 0;
  int x = 0;
  int y = 0;
  void* component = nullptr;
})
EVENT(ButtonEvent, {
  const unsigned long win = 0;
  const unsigned int button = 0;
  const int x = 0;
  const int y = 0;
  const bool pressed = false;
  const bool released = false;
  const bool holding = false;
})
EVENT(ScrollEvent, {
  unsigned long win = 0;
  int dy = 0;
  int dx = 0;
  int x = 0;
  int y = 0;
})
EVENT(MotionEvent, {
  unsigned long win = 0;
  int x = 0;
  int y = 0;
  bool enter = false;
  bool exit = false;
  
  bool dragging = false;
})
EVENT(ResizeEvent, {
  unsigned long win = 0;
  int w = 0;
  int h = 0;
})

enum Key {
  UNKNOWN,
  A,
  B,
  C,
  D,
  E,
  F,
  G,
  H,
  I,
  J,
  K,
  L,
  M,
  N,
  O,
  P,
  Q,
  R,
  S,
  T,
  U,
  V,
  W,
  X,
  Y,
  Z,
  SPACE,
  ENTER,
  BACKSPACE,
  DELETE,
  ESC,
  LEFT,
  RIGHT,
  UP,
  DOWN,
  LEFT_CTRL,
  RIGHT_CTRL,
  LEFT_SHIFT,
  RIGHT_SHIFT,
  LEFT_SUPER,
  RIGHT_SUPER,
};

EVENT(KeyEvent, {
  const unsigned long win = 0;
  const Key key;
  const bool pressed = false;
  const bool released = false;
  const bool holding = false;
  const str text = "";
})

#endif //CYD_UI_CYDUI_EVENTS_H
