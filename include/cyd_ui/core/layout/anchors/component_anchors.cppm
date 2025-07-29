// Copyright (c) 2025, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#define TO_STRING(...) #__VA_ARGS__
#define ANCHOR(PREFIX, NAME)                                                                       \
  namespace NAME {                                                                                 \
    const dimension_parameter_t x {TO_STRING(PREFIX##_##NAME##_x)};                               \
    const dimension_parameter_t y {TO_STRING(PREFIX##_##NAME##_y)};                               \
  }


export module cydui.geometry.anchors;

export import std;

export import cydui.dimensions;

export namespace cydui::layout::anchors {
  using dimension_parameter_t = dimensions::parameter<dimensions::screen_measure>;
  namespace self_component {
    const dimension_parameter_t x {"self_x"};
    const dimension_parameter_t y {"self_y"};
    const dimension_parameter_t width {"self_width"};
    const dimension_parameter_t height {"self_height"};
    const dimension_parameter_t screen_x {"self_screen_x"};
    const dimension_parameter_t screen_y {"self_screen_y"};
    const dimension_parameter_t content_width {"self_content_width"};
    const dimension_parameter_t content_height {"self_content_height"};
  }; // namespace self_component

  namespace parent_component {
    const dimension_parameter_t x {"parent_x"};
    const dimension_parameter_t y {"parent_y"};
    const dimension_parameter_t width {"parent_width"};
    const dimension_parameter_t height {"parent_height"};

    ANCHOR(
      parent,
      top_left);

    ANCHOR(
      parent,
      top_center);

    ANCHOR(
      parent,
      top_right);

    ANCHOR(
      parent,
      middle_left);

    ANCHOR(
      parent,
      center);

    ANCHOR(
      parent,
      middle_right);

    ANCHOR(
      parent,
      bottom_left);

    ANCHOR(
      parent,
      bottom_center);

    ANCHOR(
      parent,
      bottom_right);
  }; // namespace parent_component

  namespace previous_component {
    const dimension_parameter_t x {"prev_x"};
    const dimension_parameter_t y {"prev_y"};
    const dimension_parameter_t width {"prev_width"};
    const dimension_parameter_t height {"prev_height"};

    ANCHOR(
      prev,
      top_left);

    ANCHOR(
      prev,
      top_center);

    ANCHOR(
      prev,
      top_right);

    ANCHOR(
      prev,
      middle_left);

    ANCHOR(
      prev,
      center);

    ANCHOR(
      prev,
      middle_right);

    ANCHOR(
      prev,
      bottom_left);

    ANCHOR(
      prev,
      bottom_center);

    ANCHOR(
      prev,
      bottom_right);
  }; // namespace previous_component
} // namespace cydui::layout::anchors
