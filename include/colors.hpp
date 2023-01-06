//
// Created by castle on 8/24/22.
//

#ifndef CYD_UI_COLORS_HPP
#define CYD_UI_COLORS_HPP

#include <string>

namespace cydui::layout::color {
  class Color {
    std::string hex;

  public:
    explicit Color(std::string color);

    std::string to_string();
  };
}// namespace cydui::layout::color

namespace cydui::c {
  extern cydui::layout::color::Color White;
  extern cydui::layout::color::Color Black;
  extern cydui::layout::color::Color Red;
  extern cydui::layout::color::Color Green;
  extern cydui::layout::color::Color Blue;
}// namespace cydui::c

#endif//CYD_UI_COLORS_HPP
