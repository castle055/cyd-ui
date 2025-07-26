/*! \file  geometric_relations.cppm
 *! \brief
 *!
 */

export module cydui.geometric_relations;

import std;
export import reflect;

export import cydui.core.mounted;
export import cydui.geometry;
export import cydui.dimensions.functions;

export namespace cydui::layout {
  void update_content_size_in_axis(
    core::mounted_component_t& component,
    axis                             axis
  );

  void update_content_size(core::mounted_component_t& component);

  void update_component_geometry(core::mounted_component_t& component);

  core::mounted_component_t* find_by_coords(
    core::mounted_component_t& component,
    dimension_t::value_type          x,
    dimension_t::value_type          y
  );

  void compute_geometry(core::mounted_component_t& component);
} // namespace cydui::geometry
