//
// Created by castle on 10/6/23.
//

#ifndef CYD_UI_PIXELMAP_H
#define CYD_UI_PIXELMAP_H

#include "cydstd/multidim_data.h"

struct __attribute__((packed)) pixel_t {
  u8 b;
  u8 g;
  u8 r;
  u8 a;
};

using pixelmap_t = md_buffer_t<pixel_t, 2>;

using image_t = pixelmap_t;

#endif //CYD_UI_PIXELMAP_H
