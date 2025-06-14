/*! \file  geometric_relations.cppm
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

export module cydui.geometric_relations;

import std;
export import reflect;

export import cydui.components.mounted;
export import cydui.geometry;

namespace cydui::geometry {
  export void update_content_size_in_axis(
    components::mounted_component_t& component,
    axis                             axis
  ) {
    auto& geom = component.get_geometry();

    std::vector<dimension_t>                                                      dims{};
    std::unordered_set<dimensions::expression<dimensions::screen_measure>::dep_t> deps{};
    for (auto& child: component.get_children()) {
      auto& c_geom = child->get_geometry();
      if (c_geom.positioning[axis] == component_positioning::RELATIVE) {
        dims.emplace_back(c_geom.position[axis] + c_geom.screen_size[axis]);
        deps.insert(c_geom.position[axis].as_dependency());
        deps.insert(c_geom.screen_size[axis].as_dependency());
      }
    }
    geom.content_size[axis] = dimensions::function<dimensions::screen_measure>{
      [=] {
        auto max = 0_px;
        for (auto d: dims) {
          dimensions::compute(d);
          max = std::max(max, dimensions::get_value(d));
        }
        return max;
      },
      deps
    };
  }

  export void update_content_size(components::mounted_component_t& component) {
    update_content_size_in_axis(component, X_AXIS);
    update_content_size_in_axis(component, Y_AXIS);
  }

  export void update_component_geometry(components::mounted_component_t& component) {
    component_geometry&             geom  = component.get_geometry();
    const components::style_base_t& style = component.get_style();

    //* Position
    geom.position[X_AXIS] = style.x;
    if (style.position_x == position_e::ABSOLUTE or component.is_root()) {
      geom.set_position_absolute(X_AXIS);
    } else if (style.position_x == position_e::RELATIVE and not component.is_root()) {
      geom.set_position_relative(X_AXIS, component.get_parent()->get_geometry());
    }
    geom.position[Y_AXIS] = style.y;
    if (style.position_y == position_e::ABSOLUTE or component.is_root()) {
      geom.set_position_absolute(Y_AXIS);
    } else if (style.position_y == position_e::RELATIVE and not component.is_root()) {
      geom.set_position_relative(Y_AXIS, component.get_parent()->get_geometry());
    }

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

  export components::mounted_component_t* find_by_coords(
    components::mounted_component_t& component,
    dimension_t::value_type          x,
    dimension_t::value_type          y
  ) {
    using namespace dimensions;

    const auto& geom  = component.get_geometry();
    const auto& style = component.get_style();

    if (style.overflow_x != overflow_e::GROW) {
      if (not geom.box_contains_point_in_axis(geometry::X_AXIS, x)) {
        return nullptr;
      }
    }
    if (style.overflow_y != overflow_e::GROW) {
      if (not geom.box_contains_point_in_axis(geometry::Y_AXIS, y)) {
        return nullptr;
      }
    }

    components::mounted_component_t* found = nullptr;
    if (geom.viewport_contains_point(x, y)) {
      for (auto c = component.get_children().rbegin(); c != component.get_children().rend(); ++c) {
        // iterator to unique_ptr -> double trouble (dereferencing)
        found = find_by_coords(*(*c), x, y);
        if (nullptr != found) {
          return found;
        }
      }
    }

    if (not geom.box_contains_point(x, y)) {
      return nullptr;
    }
    return &component;
  }

  export void compute_geometry(components::mounted_component_t& component) {
    bool changed = component.get_geometry().compute_dimensions();
    if (changed) {
      component.get_compositing_node().queue_graphics_update();
    }

    for (auto& child: component.get_children()) {
      compute_geometry(*child);
    }
  }
} // namespace cydui::geometry
