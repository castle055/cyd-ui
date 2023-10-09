//
// Created by castle on 10/6/23.
//

#ifndef CYD_UI_PIXELMAP_EDITOR_H
#define CYD_UI_PIXELMAP_EDITOR_H

#include "pixelmap.h"

class pixelmap_editor_t {
  pixelmap_t &img;
public:
  explicit pixelmap_editor_t(pixelmap_t &img): img(img) {
  
  }

private: // ? Draw State
  pixel_t draw_color {.g = 255};

public: // ? Draw Functions
  void set_color(pixel_t pixel_color);
  
  struct line_options {
    unsigned int stroke_width = 1;
  };
  
  void draw_line(
    int x0,
    int y0,
    int x1,
    int y1,
    line_options options = {.stroke_width = 1}
  );
  
  void draw_rectangle();
  
  void draw_polygon();
  
  void draw_circle();
  
  void draw_arc();

public: // ? Clear Functions
  void clear_line();
  
  void clear_rectangle();
  
  void clear_polygon();
  
  void clear_circle();
  
  void clear_arc();

public: // ? Post-Processing
  
  void apply_edge_blur();
};

#endif //CYD_UI_PIXELMAP_EDITOR_H
