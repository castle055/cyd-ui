// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

export module cydui.graphics.types:color;

import std;

import fabric.type_aliases;
import fabric.char_utils;
import fabric.templates.str_buffer;

export namespace color {
  struct Color {
    double r = 0.0;
    double g = 0.0;
    double b = 0.0;
    double a = 1.0;

    [[nodiscard("Don't call 'to_string()' if you don't need a string.")]]
    std::string to_string() const {
      std::ostringstream oss;
      oss << '#';
      oss << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(r * 255.0);
      oss << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(g * 255.0);
      oss << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(b * 255.0);
      return oss.str();
    }

    u32 to_id() const {
      u32 id = 0;
      return id | ((u32)r << 16) | ((u32)g << 8) | (u32)b;
    }

    bool operator==(const Color& rhl) const {
      return r == rhl.r && g == rhl.g && b == rhl.b && a == rhl.a;
    }
  };
}

export template<template_str_buffer data>
consteval color::Color operator ""_color() {
  static_assert(data.count == 7 || data.count == 9, "Invalid Color, wrong length");
  static_assert(data.data[0] == '#', "Invalid Color, must begin with '#'");

  static_assert(char_is_hex(data.data[1]), "Invalid char (1), must be hexadecimal");
  static_assert(char_is_hex(data.data[2]), "Invalid char (2), must be hexadecimal");
  static_assert(char_is_hex(data.data[3]), "Invalid char (3), must be hexadecimal");
  static_assert(char_is_hex(data.data[4]), "Invalid char (4), must be hexadecimal");
  static_assert(char_is_hex(data.data[5]), "Invalid char (5), must be hexadecimal");
  static_assert(char_is_hex(data.data[6]), "Invalid char (6), must be hexadecimal");
  if constexpr (data.count == 9) {
    static_assert(char_is_hex(data.data[7]), "Invalid char (7), must be hexadecimal");
    static_assert(char_is_hex(data.data[8]), "Invalid char (8), must be hexadecimal");
  }

  u8 r = 0U;
  (void) std::from_chars(&data.data[1], &data.data[3], r, 16);
  u8 g = 0U;
  (void) std::from_chars(&data.data[3], &data.data[5], g, 16);
  u8 b = 0U;
  (void) std::from_chars(&data.data[5], &data.data[7], b, 16);
  if (data.count == 9) {
    u8 a = 0U;
    (void) std::from_chars(&data.data[7], &data.data[9], a, 16);
    return {(double) r / 255.0, (double) g / 255.0, (double) b / 255.0, (double) a / 255.0};
  } else {
    return {(double) r / 255.0, (double) g / 255.0, (double) b / 255.0, 1.0};
  }
}

export namespace color {
  Color White       = "#FFFFFF"_color;
  Color Black       = "#000000"_color;
  Color Red         = "#FF0000"_color;
  Color Green       = "#00FF00"_color;
  Color Blue        = "#0000FF"_color;
  Color Transparent = "#00000000"_color;
}
