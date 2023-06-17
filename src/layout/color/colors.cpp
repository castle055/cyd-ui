//
// Created by castle on 8/24/22.
//

#include <iomanip>
#include "color.h"

str color::Color::to_string() const {
  std::ostringstream oss;
  oss << '#';
  oss << std::setfill('0') << std::setw(2) << std::hex
      << static_cast<int>(r) << static_cast<int>(g) << static_cast<int>(b);
  return oss.str();
}

color::Color color::White = "#FFFFFF"_color;
color::Color color::Black = "#000000"_color;
color::Color color::Red = "#FF0000"_color;
color::Color color::Green = "#00FF00"_color;
color::Color color::Blue = "#0000FF"_color;
