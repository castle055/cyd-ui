//
// Created by castle on 6/17/23.
//

#ifndef CYD_UI_COLOR_H
#define CYD_UI_COLOR_H

#include <charconv>
#include "cydstd/cydstd.h"
#include "cydstd/template_str_buffer.h"
#include "cydstd/assert.h"

namespace color {
    struct Color {
      uint8_t r = 0U;
      uint8_t g = 0U;
      uint8_t b = 0U;
      
      [[nodiscard("Don't call 'to_string()' if you don't need a string.")]]
      str to_string() const;
      
      u32 to_id() const;
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
  
  uint8_t r = 0U;
  (void) std::from_chars(&data.data[1], &data.data[3], r, 16);
  uint8_t g = 0U;
  (void) std::from_chars(&data.data[3], &data.data[5], g, 16);
  uint8_t b = 0U;
  (void) std::from_chars(&data.data[5], &data.data[7], b, 16);
  return {r, g, b};
}

#endif //CYD_UI_COLOR_H
