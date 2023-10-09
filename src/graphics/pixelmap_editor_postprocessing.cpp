//
// Created by castle on 10/6/23.
//

#include "graphics/pixelmap_editor.h"

void pixelmap_editor_t::apply_edge_blur() {
  size_t w = img.size[0];
  size_t h = img.size[1];
  
  for (size_t y = 1; y < h - 1; ++y) {
    for (size_t x = 1; x < w - 1; ++x) {
      pixel_t* center = img.get({x, y});
      pixel_t* north = img.get({x, y - 1});
      pixel_t* south = img.get({x, y + 1});
      pixel_t* east = img.get({x + 1, y});
      pixel_t* west = img.get({x - 1, y});
      
      double r_avg = 0.0
        + .5 * center->r
        + .125 * north->r
        + .125 * south->r
        + .125 * east->r
        + .125 * west->r;
      double g_avg = 0.0
        + .5 * center->g
        + .125 * north->g
        + .125 * south->g
        + .125 * east->g
        + .125 * west->g;
      double b_avg = 0.0
        + .5 * center->b
        + .125 * north->b
        + .125 * south->b
        + .125 * east->b
        + .125 * west->b;
      
      double luma_center = (center->r + center->g + center->b) / 3.0;
      double luma_min = std::min({
        luma_center,
        (north->r + north->g + north->b) / 3.0,
        (south->r + south->g + south->b) / 3.0,
        (east->r + east->g + east->b) / 3.0,
        (west->r + west->g + west->b) / 3.0,
      });
      double luma_max = std::max({
        luma_center,
        (north->r + north->g + north->b) / 3.0,
        (south->r + south->g + south->b) / 3.0,
        (east->r + east->g + east->b) / 3.0,
        (west->r + west->g + west->b) / 3.0,
      });
      double contrast = luma_max - luma_min;
      
      if (contrast > 8) {
        center->r = r_avg;
        center->g = g_avg;
        center->b = b_avg;
      }
    }
  }
}
