// Copyright (c) 2024, Victor Castillo, All rights reserved.

#ifndef CYD_UI_PIXELMAP_H
#define CYD_UI_PIXELMAP_H

#include <cyd_fabric/memory/multidim_data.h>

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

#endif //CYD_UI_PIXELMAP_H
