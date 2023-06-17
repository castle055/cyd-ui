//
// Created by castle on 6/17/23.
//

#ifndef CYD_UI_COLOR_H
#define CYD_UI_COLOR_H

#include "cydstd/cydstd.h"
#include "cydstd/template_str_buffer.h"
#include "cydstd/assert.h"

namespace color {
    struct Color {
      const char* hex;
      
      [[nodiscard("Don't call 'to_string()' if you don't need a string.")]]
      str to_string() const;
    };
    
    extern Color White;
    extern Color Black;
    extern Color Red;
    extern Color Green;
    extern Color Blue;
}

template<template_str_buffer data>
consteval color::Color operator ""_color() {
  static_assert(data.count == 7, "Invalid Color, wrong length");
  static_assert(data.data[0] == '#', "Invalid Color, must begin with '#'");
  
  STATIC_ASSERT_IS_HEX(data.data[1]);
  STATIC_ASSERT_IS_HEX(data.data[2]);
  STATIC_ASSERT_IS_HEX(data.data[3]);
  STATIC_ASSERT_IS_HEX(data.data[4]);
  STATIC_ASSERT_IS_HEX(data.data[5]);
  STATIC_ASSERT_IS_HEX(data.data[6]);
  
  return {.hex = data.data};
}

#endif //CYD_UI_COLOR_H
