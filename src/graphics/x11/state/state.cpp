// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

#include "state.hpp"

import fabric.logging;

static Display* dpy = nullptr;
static int screen = -1;

using namespace x11;

Display* state::get_dpy() {
  if (dpy)
    return dpy;
  if ((dpy = XOpenDisplay(nullptr))) {
    setlocale(LC_ALL, "");
    LOG::print{INFO}("dpy = {}", (unsigned long) dpy);
    return dpy;
  } else {
    LOG::print{ERROR}("Could not open display");
    return nullptr;
  }
}

int state::get_screen() {
  if (screen != -1)
    return screen;
  screen = DefaultScreen(get_dpy());
  LOG::print{INFO}("screen = {}", screen);
  return screen;
}

Window state::get_root() {
  Window root = XRootWindow(get_dpy(), get_screen());
  LOG::print{INFO}("root = {}", root);
  return root;
}
