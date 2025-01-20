// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include <cyd_fabric_modules/headers/macros/async_events.h>
#include <SDL2/SDL_keyboard.h>

export module cydui.events;

export import std;
export import fabric.async;
export import cydui.dimensions;

using cyd::ui::dimensions::screen_measure;

export {
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

  using Scancode = SDL_Scancode;
  using Keycode = SDL_KeyCode;
  using Keymod = Uint16;

  struct Keysym {
    Scancode scancode;
    SDL_Keycode code;
    Keymod mod;
  };

  EVENT(KeyEvent) {
    const Keysym        keysym;
    const bool          pressed  = false;
    const bool          released = false;
    const bool          holding  = false;
  };

  EVENT(TextInputEvent) {
    std::string text = "";
    bool compositing_event = false;
    struct {
      int cursor{0};
      int selection{0};
    } compositing_state { };
  };
}