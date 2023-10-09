//
// Created by castle on 10/6/23.
//

#include "graphics/pixelmap_editor.h"

void pixelmap_editor_t::set_color(pixel_t pixel_color) {
  draw_color = pixel_color;
}

void pixelmap_editor_t::draw_line(
  int x0,
  int y0,
  int x1,
  int y1,
  line_options options
) {
  if (options.stroke_width == 1) {
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    
    int x_inc = (x0 < x1) ? 1 : -1;
    int y_inc = (y0 < y1) ? 1 : -1;
    
    int err = dx - dy;
    int err2;
    int x = x0;
    int y = y0;
    
    while (x != x1 || y != y1) {
      img.set({(size_t) x, (size_t) y}, draw_color);
      
      err2 = err * 2;
      
      if (err2 > -dy) {
        err -= dy;
        x += x_inc;
      }
      if (err2 < dx) {
        err += dx;
        y += y_inc;
      }
    }
    img.set({(size_t) x1, (size_t) y1}, draw_color);
  } else {
    double tx = x1 - x0;
    double ty = y1 - y0;
    double length = sqrt(tx * tx + ty * ty);
    tx /= length;
    ty /= length;
    
    double nx = -ty;
    double ny = tx;
    
    for (unsigned int i = 0; i < options.stroke_width * 32; ++i) {
      line_options opts = options;
      opts.stroke_width = 1;
      //set_color({.b = (u8)(i/2), .r = (u8)((options.stroke_width*32 - i)/2)});
      draw_line(
        (int) (x0 + .25 + (nx / 32.0) * (i + 2)),
        (int) (y0 + .25 + (ny / 32.0) * (i + 2)),
        (int) (x1 + .25 + (nx / 32.0) * i),
        (int) (y1 + .25 + (ny / 32.0) * i),
        opts
      );
    }
  }
}

void pixelmap_editor_t::draw_rectangle() {

}

void pixelmap_editor_t::draw_polygon() {

}

void pixelmap_editor_t::draw_circle() {

}

void pixelmap_editor_t::draw_arc() {

}
