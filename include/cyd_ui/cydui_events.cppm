// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include <cyd_fabric_modules/headers/macros/async_events.h>

export module cydui.events;

export import std;
export import fabric.async;
export import cydui.dimensions;

using cyd::ui::dimensions::screen_measure;

export {
  using namespace quantify;
  EVENT(RedrawEvent) {
    unsigned long win       = 0;
    int           x         = 0;
    int           y         = 0;
    void*         component = nullptr;
  };
  EVENT(ButtonEvent) {
    const unsigned long  win      = 0;
    const unsigned int   button   = 0;
    const screen_measure x        = 0;
    const screen_measure y        = 0;
    const bool           pressed  = false;
    const bool           released = false;
    const bool           holding  = false;
  };
  EVENT(ScrollEvent) {
    unsigned long        win = 0;
    const screen_measure dy  = 0;
    const screen_measure dx  = 0;
    const screen_measure x   = 0;
    const screen_measure y   = 0;
  };
  EVENT(MotionEvent) {
    unsigned long        win      = 0;
    const screen_measure x        = 0;
    const screen_measure y        = 0;
    bool                 enter    = false;
    bool                 exit     = false;

    bool                 dragging = false;
  };
  EVENT(ResizeEvent) {
    unsigned long        win = 0;
    const screen_measure w   = 0;
    const screen_measure h   = 0;
  };

  enum class Button {
    PRIMARY   = 1,
    SECONDARY = 3,
    WHEEL     = 2,
    BUTTON1   = PRIMARY,
    BUTTON2   = WHEEL,
    BUTTON3   = SECONDARY,
  };

  enum class Key {
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
    PAGE_UP,
    PAGE_DOWN,
    HOME,
    END,
    F1,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F10,
    F11,
    F12,
    F13,
    F14,
    F15,
    F16,
    F17,
    F18,
    F19,
    F20,
    F21,
    F22,
    F23,
    F24,
    F25,
    F26,
    F27,
    F28,
    F29,
    F30,
    F31,
    F32,
    F33,
    F34,
    F35,
  };

  EVENT(KeyEvent) {
    const unsigned long win = 0;
    const Key           key;
    const bool          pressed  = false;
    const bool          released = false;
    const bool          holding  = false;
    const std::string   text     = "";
  };
}