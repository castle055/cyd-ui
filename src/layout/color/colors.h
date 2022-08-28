//
// Created by castle on 8/24/22.
//

#ifndef CYD_UI_COLORS_H
#define CYD_UI_COLORS_H

#include <string>

namespace cydui::layout::color {
  class Color {
    std::string hex;
  public:
    explicit Color(std::string color);
    
    std::string to_string();
  };
}

namespace cydui::c {
  extern cydui::layout::color::Color White;
  extern cydui::layout::color::Color Black;
  extern cydui::layout::color::Color Red;
  extern cydui::layout::color::Color Green;
  extern cydui::layout::color::Color Blue;
}

#endif //CYD_UI_COLORS_H
