//
// Created by castle on 1/16/23.
//

#ifndef CYD_UI_EVENT_TYPES_H
#define CYD_UI_EVENT_TYPES_H

#include "events.hpp"

EVENT(RedrawEvent, {
  unsigned int win = 0;
  int x = 0;
  int y = 0;
  void* component = nullptr;
})
EVENT(ButtonEvent, {
  const unsigned int win = 0;
  const unsigned int button = 0;
  const int x = 0;
  const int y = 0;
  const bool pressed = false;
  const bool released = false;
  const bool holding = false;
})
EVENT(ScrollEvent, {
  unsigned int win = 0;
  int d = 0;
  int x = 0;
  int y = 0;
})
EVENT(MotionEvent, {
  unsigned int win = 0;
  int x = 0;
  int y = 0;
  bool enter = false;
  bool exit = false;
})
EVENT(ResizeEvent, {
  unsigned int win = 0;
  int w = 0;
  int h = 0;
})

enum Key {
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
  ESC,
  LEFT,
  RIGHT,
  UP,
  DOWN,
};

EVENT(KeyEvent, {
  const unsigned int win = 0;
  const Key key;
  const bool pressed = false;
  const bool released = false;
  const bool holding = false;
  const std::string text = "";
})

#endif //CYD_UI_EVENT_TYPES_H
