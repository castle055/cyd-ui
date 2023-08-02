//
// Created by castle on 8/24/22.
//

#include <iomanip>
#include "color.h"

#include "cyd-log/logging.hpp"

logging::logger logg {.on = false};

str color::Color::to_string() const {
  std::ostringstream oss;
  oss << '#';
  oss << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(r);
  oss << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(g);
  oss << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(b);
  logg.info("Color (%d,%d,%d) -> %s", r, g, b, oss.str().c_str());
  return oss.str();
}

u32 color::Color::to_id() const {
  u32 id = 0;
  return id | ((u32) r << 16) | ((u32) g << 8) | (u32) b;
}

color::Color color::White = "#FFFFFF"_color;
color::Color color::Black = "#000000"_color;
color::Color color::Red = "#FF0000"_color;
color::Color color::Green = "#00FF00"_color;
color::Color color::Blue = "#0000FF"_color;
