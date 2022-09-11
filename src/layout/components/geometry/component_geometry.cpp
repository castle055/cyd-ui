//
// Created by castle on 9/3/22.
//

#include "component_geometry.hpp"

IntProperty::IntBinding ComponentGeometry::abs_x() {
  return x + x_off;
}

IntProperty::IntBinding ComponentGeometry::abs_y() {
  return y + y_off;
}

IntProperty::IntBinding ComponentGeometry::content_x() {
  return x + x_off + margin_left + padding_left;
}

IntProperty::IntBinding ComponentGeometry::content_y() {
  return y + y_off + margin_top + padding_top;
}

IntProperty::IntBinding ComponentGeometry::border_x() {
  return x + x_off + margin_left;
}

IntProperty::IntBinding ComponentGeometry::border_y() {
  return y + y_off + margin_top;
}


IntProperty::IntBinding ComponentGeometry::abs_w() {
  return w + padding_left + padding_right + margin_left + margin_right;
}

IntProperty::IntBinding ComponentGeometry::abs_h() {
  return h + padding_top + padding_bottom + margin_top + margin_bottom;
}

IntProperty::IntBinding ComponentGeometry::content_w() {
  return &w;
}

IntProperty::IntBinding ComponentGeometry::content_h() {
  return &h;
}

IntProperty::IntBinding ComponentGeometry::border_w() {
  return w + padding_left + padding_right;
}

IntProperty::IntBinding ComponentGeometry::border_h() {
  return h + padding_top + padding_bottom;
}


void ComponentGeometry::set_pos(ComponentGeometry* relative, int x, int y) {
  auto* xp = new IntProperty(x);
  auto* yp = new IntProperty(y);
  xp->persistent = false;
  yp->persistent = false;
  set_pos(relative, xp, yp);
}

//void ComponentGeometry::set_pos(ComponentGeometry* relative, IntProperty* x, IntProperty* y) {
//  set_pos(relative, x, x);
//}
//
void ComponentGeometry::set_pos(ComponentGeometry* relative, IntProperty::IntBinding x, IntProperty::IntBinding y) {
  if (relative) {
    x_off = relative->content_x();
    y_off = relative->content_y();
    this->x = x;
    this->y = y;
    relative_to = relative;
  } else {
    x_off       = x;
    y_off       = y;
    relative_to = nullptr;
  }
  custom_offset = true;
}


void ComponentGeometry::set_size(int w, int h) {
  auto* wp = new IntProperty(w);
  auto* hp = new IntProperty(h);
  wp->persistent = false;
  hp->persistent = false;
  set_size(wp, hp);
}

//void ComponentGeometry::set_size(IntProperty* w, IntProperty* h) {
//  set_size(w, h);
//}

void ComponentGeometry::set_size(IntProperty::IntBinding w, IntProperty::IntBinding h) {
  this->w = w - padding_left - padding_right - margin_left - margin_right;
  this->h = h - padding_top - padding_bottom - margin_top - margin_bottom;
  custom_width  = true;
  custom_height = true;
}

void ComponentGeometry::set_width(int w) {
  auto* wp = new IntProperty(w);
  wp->persistent = false;
  set_width(wp);
}

//void ComponentGeometry::set_width(IntProperty* w) {
//  set_width(IntProperty::IntBinding(w));
//}
//
void ComponentGeometry::set_width(IntProperty::IntBinding w) {
  this->w = w - padding_left - padding_right - margin_left - margin_right;
  custom_width = true;
}

void ComponentGeometry::set_height(int h) {
  auto* hp = new IntProperty(h);
  hp->persistent = false;
  set_height(hp);
}

//void ComponentGeometry::set_height(IntProperty* h) {
//  set_height(IntProperty::IntBinding(h));
//}
//
void ComponentGeometry::set_height(IntProperty::IntBinding h) {
  this->h = h - padding_top - padding_bottom - margin_top - margin_bottom;
  custom_height = true;
}
