/*! \file  geometric_relations.cppm
 *! \brief
 *!
 */

export module cydui.geometric_relations;

import std;
export import reflect;

export import cydui.core.Component.impl;
export import cydui.geometry;
export import cydui.dimensions.functions;

export namespace cydui::layout {
  void update_content_size_in_axis(
    detail::ComponentImpl& component,
    axis                             axis
  );

  void update_content_size(detail::ComponentImpl& component);

  void update_component_geometry(detail::ComponentImpl& component);

  detail::ComponentImpl* find_by_coords(
    detail::ComponentImpl& component,
    dimension_t::value_type          x,
    dimension_t::value_type          y
  );

  void compute_geometry(detail::ComponentImpl& component);
} // namespace cydui::geometry
