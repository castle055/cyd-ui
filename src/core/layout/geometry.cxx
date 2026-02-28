/*! \file  geometry.cxx
 *! \brief
 *!
 */

module cydui.geometry;

import std;
import reflect;

import cydui.dimensions;
import cydui.dimensions.functions;

using namespace cydui;
using namespace cydui::layout;

component_geometry::component_geometry(const std::string& name) {
  set_sizing_mode(X_AXIS, component_sizing::GROW);
  set_sizing_mode(Y_AXIS, component_sizing::GROW);

  set_position_relations(X_AXIS);
  set_position_relations(Y_AXIS);

  configure_context(name);
}

std::unique_ptr<component_geometry> component_geometry::make(const std::string& name) {
  auto ptr = std::make_unique<component_geometry>(name);
  return ptr;
}

void component_geometry::set_sizing_mode(
  axis             axis,
  component_sizing sizing) {
  if (this->sizing[axis] == sizing) {
    return;
  }
  this->sizing[axis] = sizing;
  set_sizing_relations(axis);
}

void component_geometry::set_position_relative(
  axis                axis,
  component_geometry& relative_to) {
  positioning[axis]     = component_positioning::RELATIVE;
  screen_position[axis] = relative_to.box_position[axis] + relative_to.content_origin[axis] + position[axis];
  set_position_relations(axis);
}

void component_geometry::set_position_absolute(axis axis) {
  if (positioning[axis] == component_positioning::ABSOLUTE) {
    return;
  }
  positioning[axis]     = component_positioning::ABSOLUTE;
  screen_position[axis] = position[axis];
  set_position_relations(axis);
}

dimension_t& component_geometry::get_padding(edge edge_) {
  switch (edge_) {
    case edge::TOP   : return padding[Y_AXIS][0];
    case edge::BOTTOM: return padding[Y_AXIS][1];
    case edge::LEFT  : return padding[X_AXIS][0];
    case edge::RIGHT : return padding[X_AXIS][1];
    default          : throw std::invalid_argument("Invalid edge");
  }
}

dimension_t& component_geometry::get_margin(edge edge_) {
  switch (edge_) {
    case edge::TOP   : return margin[Y_AXIS][0];
    case edge::BOTTOM: return margin[Y_AXIS][1];
    case edge::LEFT  : return margin[X_AXIS][0];
    case edge::RIGHT : return margin[X_AXIS][1];
    default          : throw std::invalid_argument("Invalid edge");
  }
}

dimension_t& component_geometry::get_border_width(edge edge_) {
  switch (edge_) {
    case edge::TOP   : return border_width[Y_AXIS][0];
    case edge::BOTTOM: return border_width[Y_AXIS][1];
    case edge::LEFT  : return border_width[X_AXIS][0];
    case edge::RIGHT : return border_width[X_AXIS][1];
    default          : throw std::invalid_argument("Invalid edge");
  }
}

bool component_geometry::box_contains_point_in_axis(
  axis                    axis,
  dimension_t::value_type val) const {
  if (
    val < dimensions::get_value(box_position[axis])
    || val >= (dimensions::get_value(box_position[axis]) + dimensions::get_value(box_size[axis]))) {
    return false;
  }
  return true;
}

bool component_geometry::box_contains_point(
  dimension_t::value_type x,
  dimension_t::value_type y) const {
  return box_contains_point_in_axis(X_AXIS, x) and box_contains_point_in_axis(Y_AXIS, y);
}

bool component_geometry::viewport_contains_point_in_axis(
  axis                    axis,
  dimension_t::value_type val) const {
  auto _pos  = dimensions::get_value(viewport_position[axis]) + dimensions::get_value(box_position[axis]);
  auto _size = dimensions::get_value(viewport_size[axis]);
  if (val < _pos || val >= (_pos + _size)) {
    return false;
  }
  return true;
}

bool component_geometry::viewport_contains_point(
  dimension_t::value_type x,
  dimension_t::value_type y) const {
  return viewport_contains_point_in_axis(X_AXIS, x) and viewport_contains_point_in_axis(Y_AXIS, y);
}

std::pair<
  dimension_t::value_type,
  dimension_t::value_type>
component_geometry::get_relative(
  dimension_t::value_type screen_x,
  dimension_t::value_type screen_y) {
  auto rel_x = screen_x - dimensions::get_value(box_position[X_AXIS]);
  auto rel_y = screen_y - dimensions::get_value(box_position[Y_AXIS]);
  return {rel_x, rel_y};
}

bool component_geometry::compute_dimensions() {
  bool changed = false;

  changed |= dimensions::compute_dimension(position[X_AXIS]).value_changed;
  changed |= dimensions::compute_dimension(position[Y_AXIS]).value_changed;
  changed |= dimensions::compute_dimension(size[X_AXIS]).value_changed;
  changed |= dimensions::compute_dimension(size[Y_AXIS]).value_changed;
  changed |= dimensions::compute_dimension(scroll[X_AXIS]).value_changed;
  changed |= dimensions::compute_dimension(scroll[Y_AXIS]).value_changed;

  changed |= dimensions::compute_dimension(screen_position[X_AXIS]).value_changed;
  changed |= dimensions::compute_dimension(screen_position[Y_AXIS]).value_changed;
  changed |= dimensions::compute_dimension(screen_size[X_AXIS]).value_changed;
  changed |= dimensions::compute_dimension(screen_size[Y_AXIS]).value_changed;
  changed |= dimensions::compute_dimension(margin_box_size[X_AXIS]).value_changed;
  changed |= dimensions::compute_dimension(margin_box_size[Y_AXIS]).value_changed;

  changed |= dimensions::compute_dimension(box_position[X_AXIS]).value_changed;
  changed |= dimensions::compute_dimension(box_position[Y_AXIS]).value_changed;
  changed |= dimensions::compute_dimension(box_size[X_AXIS]).value_changed;
  changed |= dimensions::compute_dimension(box_size[Y_AXIS]).value_changed;

  changed |= dimensions::compute_dimension(background_position[X_AXIS]).value_changed;
  changed |= dimensions::compute_dimension(background_position[Y_AXIS]).value_changed;
  changed |= dimensions::compute_dimension(background_size[X_AXIS]).value_changed;
  changed |= dimensions::compute_dimension(background_size[Y_AXIS]).value_changed;

  changed |= dimensions::compute_dimension(content_origin[X_AXIS]).value_changed;
  changed |= dimensions::compute_dimension(content_origin[Y_AXIS]).value_changed;
  changed |= dimensions::compute_dimension(content_size[X_AXIS]).value_changed;
  changed |= dimensions::compute_dimension(content_size[Y_AXIS]).value_changed;
  changed |= dimensions::compute_dimension(viewport_position[X_AXIS]).value_changed;
  changed |= dimensions::compute_dimension(viewport_position[Y_AXIS]).value_changed;
  changed |= dimensions::compute_dimension(viewport_size[X_AXIS]).value_changed;
  changed |= dimensions::compute_dimension(viewport_size[Y_AXIS]).value_changed;

  return changed;
}

void component_geometry::set_sizing_relations(axis axis) {
  using namespace dimensions;
  switch (sizing[axis]) {
    case component_sizing::FIXED:
      box_size[axis]        = size[axis];
      background_size[axis] = box_size[axis] - (border_width[axis][0] + border_width[axis][1]) / 2;
      viewport_size[axis]   = background_size[axis] - padding[axis][0] - padding[axis][1];
      margin_box_size[axis] = box_size[axis] + margin[axis][0] + margin[axis][1];
      screen_size[axis]     = margin_box_size[axis];
      break;
    case component_sizing::GROW: {
      dimension_t computed_size {dimfn::min(std::vector {max_size[axis], content_size[axis]})};
      computed_size.set_context(context, axis == X_AXIS ? "computed_width" : "computed_height");
      viewport_size[axis]   = dimfn::max(std::vector {size[axis], computed_size});
      background_size[axis] = viewport_size[axis] + padding[axis][0] + padding[axis][1];
      box_size[axis]        = background_size[axis] + (border_width[axis][0] + border_width[axis][1]) / 2;
      margin_box_size[axis] = box_size[axis] + margin[axis][0] + margin[axis][1];
      screen_size[axis]     = margin_box_size[axis];
    } break;
    default: break;
  }
}

void component_geometry::set_position_relations(axis axis) {
  box_position[axis]        = screen_position[axis] + margin[axis][0];
  background_position[axis] = border_width[axis][0] / 2;
  viewport_position[axis]   = background_position[axis] + padding[axis][0];
  content_origin[axis]      = viewport_position[axis] - scroll[axis];
}

void component_geometry::configure_context(const std::string& name) {
  context->set_name(name);

  position[X_AXIS].set_context(context, "x");
  position[Y_AXIS].set_context(context, "y");
  size[X_AXIS].set_context(context, "width");
  size[Y_AXIS].set_context(context, "height");
  scroll[X_AXIS].set_context(context, "scroll_x");
  scroll[Y_AXIS].set_context(context, "scroll_y");

  get_margin(edge::TOP).set_context(context, "margin_top");
  get_margin(edge::RIGHT).set_context(context, "margin_right");
  get_margin(edge::BOTTOM).set_context(context, "margin_bottom");
  get_margin(edge::LEFT).set_context(context, "margin_left");

  get_padding(edge::TOP).set_context(context, "padding_top");
  get_padding(edge::RIGHT).set_context(context, "padding_right");
  get_padding(edge::BOTTOM).set_context(context, "padding_bottom");
  get_padding(edge::LEFT).set_context(context, "padding_left");

  get_border_width(edge::TOP).set_context(context, "border_width_top");
  get_border_width(edge::RIGHT).set_context(context, "border_width_right");
  get_border_width(edge::BOTTOM).set_context(context, "border_width_bottom");
  get_border_width(edge::LEFT).set_context(context, "border_width_left");

  screen_position[X_AXIS].set_context(context, "screen_x");
  screen_position[Y_AXIS].set_context(context, "screen_y");
  screen_size[X_AXIS].set_context(context, "screen_width");
  screen_size[Y_AXIS].set_context(context, "screen_height");
  margin_box_size[X_AXIS].set_context(context, "margin_box_width");
  margin_box_size[Y_AXIS].set_context(context, "margin_box_height");

  box_position[X_AXIS].set_context(context, "box_x");
  box_position[Y_AXIS].set_context(context, "box_y");
  box_size[X_AXIS].set_context(context, "box_width");
  box_size[Y_AXIS].set_context(context, "box_height");

  background_position[X_AXIS].set_context(context, "background_x");
  background_position[Y_AXIS].set_context(context, "background_y");
  background_size[X_AXIS].set_context(context, "background_width");
  background_size[Y_AXIS].set_context(context, "background_height");

  content_origin[X_AXIS].set_context(context, "content_origin_x");
  content_origin[Y_AXIS].set_context(context, "content_origin_y");
  content_size[X_AXIS].set_context(context, "content_width");
  content_size[Y_AXIS].set_context(context, "content_height");
  viewport_size[X_AXIS].set_context(context, "viewport_width");
  viewport_size[Y_AXIS].set_context(context, "viewport_height");
}
