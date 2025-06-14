// Copyright (c) 2025, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
export module cydui.geometry.anchors;

export import std;

export import cydui.dimensions;

#define TO_STRING(...) #__VA_ARGS__
#define ANCHOR(PREFIX, NAME) \
  struct NAME { \
    static constexpr dimension_parameter_t x{TO_STRING(PREFIX##_##NAME##_x)}; \
    static constexpr dimension_parameter_t y{TO_STRING(PREFIX##_##NAME##_y)}; \
  }

export
{
  namespace cydui::geometry::anchors {
    using dimension_parameter_t = dimensions::parameter<dimensions::screen_measure>;
    struct self_component {
      static constexpr dimension_parameter_t x {"self_x"};
      static constexpr dimension_parameter_t y {"self_y"};
      static constexpr dimension_parameter_t width {"self_width"};
      static constexpr dimension_parameter_t height {"self_height"};
      static constexpr dimension_parameter_t screen_x {"self_screen_x"};
      static constexpr dimension_parameter_t screen_y {"self_screen_y"};
      static constexpr dimension_parameter_t content_width {"self_content_width"};
      static constexpr dimension_parameter_t content_height {"self_content_height"};
    };

    struct parent_component {
      static constexpr dimension_parameter_t x {"parent_x"};
      static constexpr dimension_parameter_t y {"parent_y"};
      static constexpr dimension_parameter_t width {"parent_width"};
      static constexpr dimension_parameter_t height {"parent_height"};

      ANCHOR(parent, top_left);

      ANCHOR(parent, top_center);

      ANCHOR(parent, top_right);

      ANCHOR(parent, middle_left);

      ANCHOR(parent, center);

      ANCHOR(parent, middle_right);

      ANCHOR(parent, bottom_left);

      ANCHOR(parent, bottom_center);

      ANCHOR(parent, bottom_right);
    };

    struct previous_component {
      static constexpr dimension_parameter_t x {"prev_x"};
      static constexpr dimension_parameter_t y {"prev_y"};
      static constexpr dimension_parameter_t width {"prev_width"};
      static constexpr dimension_parameter_t height {"prev_height"};

      ANCHOR(prev, top_left);

      ANCHOR(prev, top_center);

      ANCHOR(prev, top_right);

      ANCHOR(prev, middle_left);

      ANCHOR(prev, center);

      ANCHOR(prev, middle_right);

      ANCHOR(prev, bottom_left);

      ANCHOR(prev, bottom_center);

      ANCHOR(prev, bottom_right);
    };
  } // namespace cydui::components
}
