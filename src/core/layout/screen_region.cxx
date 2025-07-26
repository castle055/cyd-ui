/*! \file  geometric_relations.cxx
 *! \brief
 *!
 */

module cydui.screen_region;

import std;
import reflect;

import cydui.geometry;

using namespace cydui;
using namespace cydui::geometry;


screen_region_t::screen_region_t(
  dimension_t::value_type x_,
  dimension_t::value_type y_,
  dimension_t::value_type w_,
  dimension_t::value_type h_
)
    : x(x_),
      y(y_),
      w(w_),
      h(h_) {}

screen_region_t::screen_region_t(const component_geometry& geometry)
    : x(dimensions::get_value(geometry.screen_position[X_AXIS])),
      y(dimensions::get_value(geometry.screen_position[Y_AXIS])),
      w(dimensions::get_value(geometry.screen_size[X_AXIS])),
      h(dimensions::get_value(geometry.screen_size[Y_AXIS])) {}

std::optional<screen_region_t> screen_region_t::clip_with(const screen_region_t& other) const {
  if ((x >= (other.x + other.w)) or (y >= (other.y + other.h)) or ((x + w) < other.x)
      or ((y + h) < other.y)) {
    return std::nullopt;
  }

  screen_region_t sr{};

  if (x >= other.x) {
    sr.x = x;
    sr.w = std::min((other.x + other.w) - x, w);
  } else {
    sr.x = other.x;
    sr.w = std::min((x + w) - other.x, other.w);
  }

  if (y >= other.y) {
    sr.y = y;
    sr.h = std::min((other.y + other.h) - y, h);
  } else {
    sr.y = other.y;
    sr.h = std::min((y + h) - other.y, other.h);
  }

  return sr;
}

bool screen_region_t::contains_point(
  dimension_t::value_type _x,
  dimension_t::value_type _y
) const {
  return (_x >= x) and (_x < (x + w)) and (_y >= y) and (_y < (y + h));
}
