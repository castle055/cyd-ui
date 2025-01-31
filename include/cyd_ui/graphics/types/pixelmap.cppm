// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

export module cydui.graphics.types:pixelmap;

import std;

import fabric.memory.multidim_data;
import fabric.type_aliases;

export {
struct __attribute__((packed)) pixel_t {
  u8 b;
  u8 g;
  u8 r;
  u8 a;
};

struct pixelmap_t: public md_buffer_t<pixel_t, 2> {
  pixelmap_t(unsigned long w, unsigned long h): md_buffer_t<pixel_t, 2>({w, h}) {
  }
  pixelmap_t(unsigned long w, unsigned long h, pixel_t*& data): md_buffer_t<pixel_t, 2>({w, h}, data) {
  }

  [[nodiscard]] size_t width() const {
    return size[0];
  }

  [[nodiscard]] size_t height() const {
    return size[1];
  }
};

using image_t = pixelmap_t;

}
