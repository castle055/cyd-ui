/*! \file  geometry.cppm
 *! \brief
 *!
 */

export module cydui.geometry;

import std;
export import reflect;

export import cydui.dimensions;
export import cydui.dimensions.functions;

export namespace cydui {
  enum class edge { TOP, RIGHT, BOTTOM, LEFT };
}

namespace cydui::layout {
  export constexpr std::size_t AXIS_COUNT = 2;

  export using axis            = std::size_t;
  export constexpr axis X_AXIS = 0;
  export constexpr axis Y_AXIS = 1;

  export enum class component_positioning {
    RELATIVE = 0,
    ABSOLUTE = 1,
  };

  export enum class component_sizing {
    FIXED = 0,
    GROW  = 1,
  };

  export struct component_geometry {
    std::shared_ptr<dimension_ctx_t> context{std::make_shared<dimension_ctx_t>()};

    //** Inputs
    std::array<dimension_t, AXIS_COUNT>                position{};
    std::array<dimension_t, AXIS_COUNT>                size{};
    std::array<dimension_t, AXIS_COUNT>                max_size{};
    std::array<dimension_t, AXIS_COUNT>                content_size{};
    std::array<dimension_t, AXIS_COUNT>                scroll{};
    std::array<std::array<dimension_t, 2>, AXIS_COUNT> margin{};
    std::array<std::array<dimension_t, 2>, AXIS_COUNT> padding{};
    std::array<std::array<dimension_t, 2>, AXIS_COUNT> border_width{};

    //** Computed
    std::array<dimension_t, AXIS_COUNT> screen_position{};
    std::array<dimension_t, AXIS_COUNT> box_position{};
    std::array<dimension_t, AXIS_COUNT> background_position{};
    std::array<dimension_t, AXIS_COUNT> content_origin{};
    std::array<dimension_t, AXIS_COUNT> viewport_position{};

    std::array<dimension_t, AXIS_COUNT> screen_size{};
    std::array<dimension_t, AXIS_COUNT> margin_box_size{};
    std::array<dimension_t, AXIS_COUNT> box_size{};
    std::array<dimension_t, AXIS_COUNT> background_size{};
    std::array<dimension_t, AXIS_COUNT> viewport_size{};

    component_sizing      sizing[AXIS_COUNT]{};
    component_positioning positioning[AXIS_COUNT]{};

    explicit component_geometry(const std::string& name = "");

    static std::unique_ptr<component_geometry> make(const std::string& name = "");

    void set_sizing_mode(
      axis             axis,
      component_sizing sizing
    );

    void set_position_relative(
      axis                axis,
      component_geometry& relative_to
    );

    void set_position_absolute(axis axis);

    dimension_t& get_padding(edge edge_);

    dimension_t& get_margin(edge edge_);

    dimension_t& get_border_width(edge edge_);

    bool box_contains_point_in_axis(
      axis                    axis,
      dimension_t::value_type val
    ) const;

    bool box_contains_point(
      dimension_t::value_type x,
      dimension_t::value_type y
    ) const;

    bool viewport_contains_point_in_axis(
      axis                    axis,
      dimension_t::value_type val
    ) const;

    bool viewport_contains_point(
      dimension_t::value_type x,
      dimension_t::value_type y
    ) const;

    std::pair<
      dimension_t::value_type,
      dimension_t::value_type>
    get_relative(
      dimension_t::value_type screen_x,
      dimension_t::value_type screen_y
    );

    bool compute_dimensions();

  private:
    void set_sizing_relations(axis axis);
    void set_position_relations(axis axis);

    void configure_context(const std::string& name);
  };
} // namespace cydui::layout
