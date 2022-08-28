//
// Created by castle on 8/22/22.
//

#include "../../../include/primitives.h"
#include "../../../include/window_types.h"
#include "../../logging/logging.h"

using namespace primitives;

const logging::logger log = { .name = "COMP::PRIM::LINE", .on = false};

Line::Line(Color* color, int x1, int y1, int x2, int y2)
    : Component(std::vector<Component*>()) {
  this->x1 = x1;
  this->y1 = y1;
  this->x2 = x2;
  this->y2 = y2;
  this->color = color;
  state->w = x2;
  state->h = y2;
  log.info("LINE -> props, pos(x=%d,y=%d) size(w=%d,h=%d)", x1, y1, x2, y2);
}

void Line::on_redraw(CLayoutEvent* ev) {
  auto* win_ref = ((window::CWindow*)ev->win)->win_ref;
  auto* state = this->state;
  
  log.info("props, pos(x=%d,y=%d) size(w=%d,h=%d)", x1, y1, x2, y2);
  log.info("state, pos(x=%d,y=%d)", state->x, state->y);
  drw_line(win_ref, color, state->x + x1, state->y + y1, state->x + x2, state->y + y2);
}

Rectangle::Rectangle(Color *color, int x, int y, int w, int h, bool filled)
    : Component(std::vector<Component*>()) {
  this->x = x;
  this->y = y;
  this->w = w;
  this->h = h;
  this->color = color;
  this->filled = filled;
  state->w = x + w;
  state->h = y + h;
}

void Rectangle::on_redraw(CLayoutEvent* ev) {
  auto* win_ref = ((window::CWindow*)ev->win)->win_ref;
  auto* state = this->state;
  graphics::drw_rect(win_ref, color, state->x + x, state->y + y, w, h, filled);
}

Arc::Arc(Color *color, int x, int y, int w, int h, int a0, int a1, bool filled)
    : Component(std::vector<Component*>()) {
  this->x = x;
  this->y = y;
  this->w = w;
  this->h = h;
  this->a0 = a0;
  this->a1 = a1;
  this->color = color;
  this->filled = filled;
  state->w = x + w;
  state->h = y + h;
}

void Arc::on_redraw(CLayoutEvent* ev) {
  auto* win_ref = ((window::CWindow*)ev->win)->win_ref;
  auto* state = this->state;
  graphics::drw_arc(win_ref, color, state->x + x, state->y + y, w, h, a0, a1, filled);
}

Circle::Circle(Color *color, int x, int y, int w, int h, bool filled)
    : Component(std::vector<Component*>()) {
  this->x = x;
  this->y = y;
  this->w = w;
  this->h = h;
  this->color = color;
  this->filled = filled;
  state->w = x + w;
  state->h = y + h;
}

void Circle::on_redraw(CLayoutEvent* ev) {
  auto* win_ref = ((window::CWindow*)ev->win)->win_ref;
  auto* state = this->state;
  graphics::drw_arc(win_ref, color, state->x + x, state->y + y, w, h, 0, 360, filled);
}

