//
// Created by castle on 6/17/23.
//

#ifndef CYD_UI_FONT_H
#define CYD_UI_FONT_H

#include "cydstd/cydstd.h"
#include "cydstd/template_str_buffer.h"

namespace font {
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
