// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CYD_UI_FONT_H
#define CYD_UI_FONT_H

#include <cyd_fabric/templates/template_str_buffer.h>

namespace cyd::ui::graphics::font {
    struct Font {
      std::string name;
      int size;
      
      bool antialias = true;
      bool autohint = true;
    };
    
    //template<template_str_buffer data>
    //consteval inline Font operator ""_font() {
    //
    //  return {.hex = data.data};
    //}
    //
}

#endif //CYD_UI_FONT_H
