//
// Created by castle on 10/6/23.
//

#ifndef CYD_UI_PIXELMAP_H
#define CYD_UI_PIXELMAP_H

#include "../cydstd/multidim_data.h"

struct __attribute__((packed)) pixel_t {
  u8 b;
  u8 g;
  u8 r;
  u8 a;
};

struct pixelmap_t: public md_buffer_t<pixel_t, 2> {
  pixelmap_t(unsigned long w, unsigned long h): md_buffer_t<pixel_t, 2>({w, h}) {
  }
  
  size_t width() {
    return size[0];
  }
  
  size_t height() {
    return size[1];
  }
};

using image_t = pixelmap_t;

#endif //CYD_UI_PIXELMAP_H
