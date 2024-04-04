// Copyright (c) 2024, Victor Castillo, All rights reserved.

#ifndef CYD_UI_COLOR_H
#define CYD_UI_COLOR_H

#include <charconv>
#include "../cydstd/cydstd.h"
#include "../cydstd/template_str_buffer.h"
#include "../cydstd/assert.h"

namespace cydui::graphics::color {
    struct Color {
      double r = 0.0;
      double g = 0.0;
      double b = 0.0;
      double a = 1.0;
      
      [[nodiscard("Don't call 'to_string()' if you don't need a string.")]]
      str to_string() const;
      
      u32 to_id() const;
    };
    
    extern Color White;
    extern Color Black;
    extern Color Red;
    extern Color Green;
    extern Color Blue;
    extern Color Transparent;
}

template<template_str_buffer data>
consteval cydui::graphics::color::Color operator ""_color() {
  static_assert(data.count == 7 || data.count == 9, "Invalid Color, wrong length");
  static_assert(data.data[0] == '#', "Invalid Color, must begin with '#'");
  
  STATIC_ASSERT_IS_HEX(data.data[1]);
  STATIC_ASSERT_IS_HEX(data.data[2]);
  STATIC_ASSERT_IS_HEX(data.data[3]);
  STATIC_ASSERT_IS_HEX(data.data[4]);
  STATIC_ASSERT_IS_HEX(data.data[5]);
  STATIC_ASSERT_IS_HEX(data.data[6]);
  if constexpr (data.count == 9) {
    STATIC_ASSERT_IS_HEX(data.data[7]);
    STATIC_ASSERT_IS_HEX(data.data[8]);
  }
  
  uint8_t r = 0U;
  (void) std::from_chars(&data.data[1], &data.data[3], r, 16);
  uint8_t g = 0U;
  (void) std::from_chars(&data.data[3], &data.data[5], g, 16);
  uint8_t b = 0U;
  (void) std::from_chars(&data.data[5], &data.data[7], b, 16);
  if (data.count == 9) {
    uint8_t a = 0U;
    (void) std::from_chars(&data.data[7], &data.data[9], a, 16);
    return {(double) r / 255.0, (double) g / 255.0, (double) b / 255.0, (double) a / 255.0};
  } else {
    return {(double) r / 255.0, (double) g / 255.0, (double) b / 255.0, 1.0};
  }
}

#endif //CYD_UI_COLOR_H
