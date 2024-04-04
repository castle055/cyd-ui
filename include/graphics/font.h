// Copyright (c) 2024, Victor Castillo, All rights reserved.

#ifndef CYD_UI_FONT_H
#define CYD_UI_FONT_H

#include "../cydstd/cydstd.h"
#include "../cydstd/template_str_buffer.h"

namespace cydui::graphics::font {
    struct Font {
      str name;
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
