//
// Created by castle on 8/24/22.
//

#include "colors.h"

using namespace cydui::layout::color;

Color::Color(std::string color): hex(color) {

}

std::string Color::to_string() {
  return hex;
}

cydui::layout::color::Color cydui::c::White   ("#FFFFFF");
cydui::layout::color::Color cydui::c::Black   ("#000000");
cydui::layout::color::Color cydui::c::Red     ("#FF0000");
cydui::layout::color::Color cydui::c::Green   ("#00FF00");
cydui::layout::color::Color cydui::c::Blue    ("#0000FF");
