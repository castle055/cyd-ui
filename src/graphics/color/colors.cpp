// Copyright (c) 2024, Victor Castillo, All rights reserved.

//
// Created by castle on 8/24/22.
//

#include <iomanip>
#include "graphics/color.h"

#include <sstream>
#include <cyd_fabric/logging/logging.hpp>

using namespace cyd::ui::graphics::color;
namespace color = cyd::ui::graphics::color;

logging::logger logg {.on = false};

str Color::to_string() const {
  std::ostringstream oss;
  oss << '#';
  oss << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(r * 255.0);
  oss << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(g * 255.0);
  oss << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(b * 255.0);
  logg.info("Color (%f,%f,%f) -> %s", r, g, b, oss.str().c_str());
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
