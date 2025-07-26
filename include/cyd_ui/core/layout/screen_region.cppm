/*! \file  geometric_relations.cppm
 *! \brief
 *!
 */

export module cydui.screen_region;

import std;
export import reflect;

export import cydui.geometry;

export namespace cydui::layout {
  struct screen_region_t {
    dimension_t::value_type x, y, w, h;

    screen_region_t() = default;
    screen_region_t(
      dimension_t::value_type x,
      dimension_t::value_type y,
      dimension_t::value_type w,
      dimension_t::value_type h
    );
    explicit screen_region_t(const component_geometry& geometry);

    std::optional<screen_region_t> clip_with(const screen_region_t& other) const;

    bool contains_point(
      dimension_t::value_type _x,
      dimension_t::value_type _y
    ) const;
  };
} // namespace cydui::geometry
