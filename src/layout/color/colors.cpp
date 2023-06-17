//
// Created by castle on 8/24/22.
//

#include "color.h"

str color::Color::to_string() const {
  return {hex};
}

color::Color color::White = "#FFFFFF"_color;
color::Color color::Black = "#000000"_color;
color::Color color::Red = "#FF0000"_color;
color::Color color::Green = "#00FF00"_color;
color::Color color::Blue = "#0000FF"_color;
