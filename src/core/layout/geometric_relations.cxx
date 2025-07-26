/*! \file  geometric_relations.cxx
 *! \brief
 *!
 */

module;
#define COMPUTE(DIM)                                                                               \
  {                                                                                                \
    auto compute_res = cydui::dimensions::compute_dimension(DIM);                                  \
    if (not compute_res) {                                                                         \
      return false;                                                                                \
    }                                                                                              \
  }

module cydui.geometric_relations;

import std;
import reflect;

import cydui.components.mounted;
import cydui.geometry;
import cydui.dimensions.functions;
import cydui.screen_region;

using namespace cydui;
using namespace cydui::layout;

void layout::update_content_size_in_axis(
  components::mounted_component_t& component,
  axis                             axis
) {
  auto& geom = component.get_geometry();

  std::vector<dimension_t> dims{};
  for (auto& child: component.get_children()) {
    auto& c_geom = child->get_geometry();
    if (c_geom.positioning[axis] == component_positioning::RELATIVE) {
      dims.push_back(dimension_t{c_geom.position[axis] + c_geom.screen_size[axis]});
    }
  }
  geom.content_size[axis] = dimensions::dimfn::max(dims);
}

void layout::update_content_size(components::mounted_component_t& component) {
  update_content_size_in_axis(component, X_AXIS);
  update_content_size_in_axis(component, Y_AXIS);
}

void layout::update_component_geometry(components::mounted_component_t& component) {
  component_geometry&        geom  = component.get_geometry();
  const style::style_base_t& style = component.get_style();

  //* Position
  geom.position[X_AXIS] = style.x;
  geom.position[Y_AXIS] = style.y;
  if (style.position == position_e::ABSOLUTE or component.is_root()) {
    geom.set_position_absolute(X_AXIS);
    geom.set_position_absolute(Y_AXIS);
  } else if (style.position == position_e::RELATIVE and not component.is_root()) {
    auto pgeom = component.get_parent()->get_geometry();
    geom.set_position_relative(X_AXIS, pgeom);
    geom.set_position_relative(Y_AXIS, pgeom);
  }

  //* Max Size
  geom.max_size[X_AXIS] =
    style.max_width.empty()
      ? dimensions::screen_measure{std::numeric_limits<dimensions::screen_measure::data_type>::max()
        }
      : style.max_width;
  geom.max_size[Y_AXIS] =
    style.max_height.empty()
      ? dimensions::screen_measure{std::numeric_limits<dimensions::screen_measure::data_type>::max()
        }
      : style.max_height;

  //* Size
  geom.size[X_AXIS] = style.width;
  if (style.width.empty() or style.overflow_x == overflow_e::GROW) {
    geom.set_sizing_mode(X_AXIS, component_sizing::GROW);
  } else {
    geom.set_sizing_mode(X_AXIS, component_sizing::FIXED);
  }
  geom.size[Y_AXIS] = style.height;
  if (style.height.empty() or style.overflow_y == overflow_e::GROW) {
    geom.set_sizing_mode(Y_AXIS, component_sizing::GROW);
  } else {
    geom.set_sizing_mode(Y_AXIS, component_sizing::FIXED);
  }

  //* Scroll
  geom.scroll[X_AXIS] = style.scroll_x;
  geom.scroll[Y_AXIS] = style.scroll_y;

  //* Margin
  geom.get_margin(edge::LEFT)   = style.margin.left;
  geom.get_margin(edge::RIGHT)  = style.margin.right;
  geom.get_margin(edge::BOTTOM) = style.margin.bottom;
  geom.get_margin(edge::TOP)    = style.margin.top;

  //* Padding
  geom.get_padding(edge::LEFT)   = style.padding.left;
  geom.get_padding(edge::RIGHT)  = style.padding.right;
  geom.get_padding(edge::BOTTOM) = style.padding.bottom;
  geom.get_padding(edge::TOP)    = style.padding.top;

  //* Border Width
  geom.get_border_width(edge::LEFT)   = style.border_width.left;
  geom.get_border_width(edge::RIGHT)  = style.border_width.right;
  geom.get_border_width(edge::BOTTOM) = style.border_width.bottom;
  geom.get_border_width(edge::TOP)    = style.border_width.top;
}

components::mounted_component_t* find_by_coords(
  components::mounted_component_t& component,
  dimension_t::value_type          x,
  dimension_t::value_type          y,
  screen_region_t                  screen_region
) {
  using namespace dimensions;

  components::mounted_component_t* found = nullptr;
  for (auto c = component.get_children().rbegin(); c != component.get_children().rend(); ++c) {
    auto&           style = (*c)->get_style();
    auto&           cgeom = (*c)->get_geometry();
    screen_region_t sr{cgeom};
    auto            cliped_region = sr.clip_with(screen_region);
    if (style.position == position_e::ABSOLUTE) {
      found = find_by_coords(*(*c), x, y);
    } else if (cliped_region.has_value()) {
      // iterator to unique_ptr -> double trouble (dereferencing)
      found = find_by_coords(*(*c), x, y, cliped_region.value());
    }
    if (nullptr != found) {
      return found;
    }
  }

  if (not screen_region.contains_point(x, y)) {
    return nullptr;
  }
  return &component;
}

components::mounted_component_t* layout::find_by_coords(
  components::mounted_component_t& component,
  dimension_t::value_type          x,
  dimension_t::value_type          y
) {
  using namespace dimensions;

  const auto& geom = component.get_geometry();
  return ::find_by_coords(component, x, y, screen_region_t{geom});
}

void layout::compute_geometry(components::mounted_component_t& component) {
  bool changed = component.get_geometry().compute_dimensions();
  if (changed) {
    component.get_compositing_node().queue_graphics_update();
  }

  for (auto& child: component.get_children()) {
    compute_geometry(*child);
  }
}
