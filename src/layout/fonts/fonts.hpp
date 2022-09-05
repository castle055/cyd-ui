//
// Created by castle on 8/28/22.
//

#ifndef CYD_UI_FONTS_HPP
#define CYD_UI_FONTS_HPP

#include <string>

namespace cydui::layout::fonts {
  struct Font {
    std::string name;
    int size;

    bool antialias = true;
    bool autohint  = true;
  };

}// namespace cydui::layout::fonts


#endif//CYD_UI_FONTS_HPP
