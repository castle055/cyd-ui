// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

#include <iomanip>
#include "cyd_ui/graphics/color.h"

#include <sstream>

using namespace cyd::ui::graphics::color;
namespace color = cyd::ui::graphics::color;

str Color::to_string() const {
  std::ostringstream oss;
  oss << '#';
  oss << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(r * 255.0);
  oss << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(g * 255.0);
  oss << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(b * 255.0);
  return oss.str();
}

u32 Color::to_id() const {
  u32 id = 0;
  return id | ((u32) r << 16) | ((u32) g << 8) | (u32) b;
}

Color color::White = "#FFFFFF"_color;
Color color::Black = "#000000"_color;
Color color::Red = "#FF0000"_color;
Color color::Green = "#00FF00"_color;
Color color::Blue = "#0000FF"_color;
Color color::Transparent = "#00000000"_color;
