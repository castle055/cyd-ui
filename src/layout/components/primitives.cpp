//
// Created by castle on 8/22/22.
//

#include "../../../include/primitives.h"
#include "../../../include/window_types.h"
#include "../../logging/logging.h"

using namespace primitives;

const logging::logger log = {.name = "COMP::PRIM::LINE", .on = false};

Line::Line(Color* color, int x1, int y1, int x2, int y2)
    : Component() {
  state->geom.x = x1;
  state->geom.y = y1;
  state->geom.w = x2 - x1;
  state->geom.h = y2 - y1;
  this->color   = color;
  log.info("LINE -> props, pos(x=%d,y=%d) size(w=%d,h=%d)", x1, y1, x2, y2);
}

void Line::on_redraw(CLayoutEvent* ev) {
  auto* win_ref = ((window::CWindow*)ev->win)->win_ref;
  
  //log.info("props, pos(x=%d,y=%d) size(w=%d,h=%d)", x1, y1, x2, y2);
  //log.info("state, pos(x=%d,y=%d)", state->geom.abs_x(), state->geom.abs_y());
  graphics::drw_line(
      win_ref,
      color,
      state->geom.content_x().val(),
      state->geom.content_y().val(),
      (state->geom.content_x() + state->geom.w).val(),
      (state->geom.content_y() + state->geom.h).val()
  );
}

Rectangle::Rectangle(Color* color, int x, int y, int w, int h, bool filled)
    : Component() {
  state->geom.x           = x;
  state->geom.y           = y;
  state->geom.w           = w;
  state->geom.h           = h;
  //state->geom.custom_offset = true;
  state->geom.custom_size = true;
  this->color             = color;
  this->filled            = filled;
}

void Rectangle::on_redraw(CLayoutEvent* ev) {
  auto* win_ref = ((window::CWindow*)ev->win)->win_ref;
  graphics::drw_rect(
      win_ref,
      color,
      state->geom.content_x().val(),
      state->geom.content_y().val(),
      state->geom.w.val(),
      state->geom.h.val(),
      filled
  );
}

Arc::Arc(Color* color, int x, int y, int w, int h, int a0, int a1, bool filled)
    : Component() {
  state->geom.x = x;
  state->geom.y = y;
  state->geom.w = w;
  state->geom.h = h;
  this->a0      = a0;
  this->a1      = a1;
  this->color   = color;
  this->filled  = filled;
}

void Arc::on_redraw(CLayoutEvent* ev) {
  auto* win_ref = ((window::CWindow*)ev->win)->win_ref;
  auto* state   = this->state;
  graphics::drw_arc(
      win_ref,
      color,
      state->geom.content_x().val(),
      state->geom.content_y().val(),
      state->geom.w.val(),
      state->geom.h.val(),
      a0, a1, filled
  );
}

Circle::Circle(Color* color, int x, int y, int w, int h, bool filled)
    : Component() {
  state->geom.x = x;
  state->geom.y = y;
  state->geom.w = w;
  state->geom.h = h;
  this->color   = color;
  this->filled  = filled;
}

void Circle::on_redraw(CLayoutEvent* ev) {
  auto* win_ref = ((window::CWindow*)ev->win)->win_ref;
  graphics::drw_arc(
      win_ref, color,
      state->geom.content_x().val(),
      state->geom.content_y().val(),
      state->geom.w.val(),
      state->geom.h.val(),
      0, 360, filled
  );
}

Text::Text(Color* color, layout::fonts::Font* font, int x, int y, std::string text)
    : Component() {
  state->geom.x = x;
  state->geom.y = y;
  state->geom.w = 125;
  state->geom.h = font->size;
  this->color   = color;
  this->font    = font;
  this->text    = text;
  //state->w    = x + 125;
  //state->h    = y;
}

void Text::on_redraw(CLayoutEvent* ev) {
  auto* win_ref = ((window::CWindow*)ev->win)->win_ref;
  graphics::drw_text(
      win_ref, font, color, text,
      state->geom.content_x().val(),
      (state->geom.content_y() + font->size).val()
  );
}
