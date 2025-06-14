// Copyright (c) 2025, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include <tracy/Tracy.hpp>

export module cydui.graphics.compositing_operation;

import std;

export namespace cydui {
  enum class overflow_e { GROW, HIDE, SCROLL };
  enum class position_e { ABSOLUTE, RELATIVE };
} // namespace cydui

export namespace cydui::compositing {
  struct compositing_operation_t {
    enum {
      OVERLAY,
    } op = OVERLAY;

    int    x, y, orig_x, orig_y;
    int    w, h;
    int    vx, vy, vw, vh;
    double rot      = 0.0;
    double scale_x  = 1.0;
    double scale_y  = 1.0;
    bool   animated = false;

    overflow_e x_overflow = overflow_e::HIDE;
    overflow_e y_overflow = overflow_e::HIDE;

    bool operator==(const compositing_operation_t& other) const {
      return x == other.x && y == other.y && orig_x == other.orig_x && orig_y == other.orig_y
             && w == other.w && h == other.h && scale_x == other.scale_x && scale_y == other.scale_y
             && rot == other.rot && animated == other.animated && x_overflow == other.x_overflow
             && y_overflow == other.y_overflow;
    }
  };
} // namespace cydui::compositing
