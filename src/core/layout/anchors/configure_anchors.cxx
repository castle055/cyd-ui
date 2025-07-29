// Copyright (c) 2025, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module cydui.geometry.configure_anchors;

import std;

import cydui.core.Component.impl;
import cydui.geometry.anchors;

using namespace cydui;
using namespace cydui::layout;

#define TO_STRING(...) #__VA_ARGS__
#define ANCHOR(PREFIX, NAME)                                                                       \
  struct NAME {                                                                                    \
    static constexpr dimension_parameter_t x{TO_STRING(PREFIX##_##NAME##_x)};                      \
    static constexpr dimension_parameter_t y{TO_STRING(PREFIX##_##NAME##_y)};                      \
  }

#define DIMENSIONAL_PARAM(PREFIX, NAME, ...)                                                       \
  ctx.set_parameter(TO_STRING(PREFIX##_##NAME), __VA_ARGS__)

#define PARENT_PARAM(NAME, ...) DIMENSIONAL_PARAM(parent, NAME, __VA_ARGS__ + 0_px)
#define SELF_PARAM(NAME, ...)   DIMENSIONAL_PARAM(self, NAME, __VA_ARGS__ + 0_px)
#define PREV_PARAM(NAME, ...)   DIMENSIONAL_PARAM(prev, NAME, __VA_ARGS__ + 0_px)

void configure_self_anchors(detail::ComponentImpl& child) {
  auto&            geom = child.get_geometry();
  dimension_ctx_t& ctx  = *geom.context;

  ctx.set_parameter("self_x", geom.position[X_AXIS]);
  ctx.set_parameter("self_y", geom.position[Y_AXIS]);
  ctx.set_parameter("self_width", geom.screen_size[X_AXIS]);
  ctx.set_parameter("self_height", geom.screen_size[Y_AXIS]);
  ctx.set_parameter("self_screen_x", geom.box_position[X_AXIS]);
  ctx.set_parameter("self_screen_y", geom.box_position[Y_AXIS]);
  ctx.set_parameter("self_content_width", geom.viewport_size[X_AXIS]);
  ctx.set_parameter("self_content_height", geom.viewport_size[Y_AXIS]);
}

void configure_parent_anchors(detail::ComponentImpl& child) {
  dimension_ctx_t& ctx = *child.get_geometry().context;

  if (not child.is_root()) {
    auto& geom = child.get_parent()->get_geometry();
    auto& cx   = geom.content_origin[X_AXIS];
    auto& cy   = geom.content_origin[Y_AXIS];
    auto& cw   = geom.viewport_size[X_AXIS];
    auto& ch   = geom.viewport_size[Y_AXIS];

    PARENT_PARAM(x, cx);
    PARENT_PARAM(y, cy);
    PARENT_PARAM(width, cw);
    PARENT_PARAM(height, ch);

    PARENT_PARAM(top_left_x, cx);
    PARENT_PARAM(top_left_y, cy);
    PARENT_PARAM(top_center_x, cx + (cw / 2));
    PARENT_PARAM(top_center_y, cy);
    PARENT_PARAM(top_right_x, cx + cw);
    PARENT_PARAM(top_right_y, cy);
    PARENT_PARAM(middle_left_x, cx);
    PARENT_PARAM(middle_left_y, cy + (ch / 2));
    PARENT_PARAM(center_x, cx + (cw / 2));
    PARENT_PARAM(center_y, cy + (ch / 2));
    PARENT_PARAM(middle_right_x, cx + cw);
    PARENT_PARAM(middle_right_y, cy + (ch / 2));
    PARENT_PARAM(bottom_left_x, cx);
    PARENT_PARAM(bottom_left_y, cy + ch);
    PARENT_PARAM(bottom_center_x, cx + (cw / 2));
    PARENT_PARAM(bottom_center_y, cy + ch);
    PARENT_PARAM(bottom_right_x, cx + cw);
    PARENT_PARAM(bottom_right_y, cy + ch);
  } else {
    PARENT_PARAM(x, 0_px);
    PARENT_PARAM(y, 0_px);
    PARENT_PARAM(width, 0_px);
    PARENT_PARAM(height, 0_px);

    PARENT_PARAM(top_left_x, 0_px);
    PARENT_PARAM(top_left_y, 0_px);
    PARENT_PARAM(top_center_x, 0_px);
    PARENT_PARAM(top_center_y, 0_px);
    PARENT_PARAM(top_right_x, 0_px);
    PARENT_PARAM(top_right_y, 0_px);
    PARENT_PARAM(middle_left_x, 0_px);
    PARENT_PARAM(middle_left_y, 0_px);
    PARENT_PARAM(center_x, 0_px);
    PARENT_PARAM(center_y, 0_px);
    PARENT_PARAM(middle_right_x, 0_px);
    PARENT_PARAM(middle_right_y, 0_px);
    PARENT_PARAM(bottom_left_x, 0_px);
    PARENT_PARAM(bottom_left_y, 0_px);
    PARENT_PARAM(bottom_center_x, 0_px);
    PARENT_PARAM(bottom_center_y, 0_px);
    PARENT_PARAM(bottom_right_x, 0_px);
    PARENT_PARAM(bottom_right_y, 0_px);
  }
}

void configure_prev_anchors(
  detail::ComponentImpl&                child,
  std::optional<detail::ComponentImpl*> prev
) {
  dimension_ctx_t& ctx = *child.get_geometry().context;

  if (prev.has_value()) {
    auto& geom = prev.value()->get_geometry();
    auto& x    = geom.position[X_AXIS];
    auto& y    = geom.position[Y_AXIS];
    auto& w    = geom.screen_size[X_AXIS];
    auto& h    = geom.screen_size[Y_AXIS];

    PREV_PARAM(x, x);
    PREV_PARAM(y, y);
    PREV_PARAM(width, w);
    PREV_PARAM(height, h);

    PREV_PARAM(top_left_x, x);
    PREV_PARAM(top_left_y, y);
    PREV_PARAM(top_center_x, x + (w / 2));
    PREV_PARAM(top_center_y, y);
    PREV_PARAM(top_right_x, x + w);
    PREV_PARAM(top_right_y, y);
    PREV_PARAM(middle_left_x, x);
    PREV_PARAM(middle_left_y, y + (h / 2));
    PREV_PARAM(center_x, x + (w / 2));
    PREV_PARAM(center_y, y + (h / 2));
    PREV_PARAM(middle_right_x, x + w);
    PREV_PARAM(middle_right_y, y + (h / 2));
    PREV_PARAM(bottom_left_x, x);
    PREV_PARAM(bottom_left_y, y + h);
    PREV_PARAM(bottom_center_x, x + (w / 2));
    PREV_PARAM(bottom_center_y, y + h);
    PREV_PARAM(bottom_right_x, x + w);
    PREV_PARAM(bottom_right_y, y + h);
  } else {
    PREV_PARAM(x, 0_px);
    PREV_PARAM(y, 0_px);
    PREV_PARAM(width, 0_px);
    PREV_PARAM(height, 0_px);

    PREV_PARAM(top_left_x, 0_px);
    PREV_PARAM(top_left_y, 0_px);
    PREV_PARAM(top_center_x, 0_px);
    PREV_PARAM(top_center_y, 0_px);
    PREV_PARAM(top_right_x, 0_px);
    PREV_PARAM(top_right_y, 0_px);
    PREV_PARAM(middle_left_x, 0_px);
    PREV_PARAM(middle_left_y, 0_px);
    PREV_PARAM(center_x, 0_px);
    PREV_PARAM(center_y, 0_px);
    PREV_PARAM(middle_right_x, 0_px);
    PREV_PARAM(middle_right_y, 0_px);
    PREV_PARAM(bottom_left_x, 0_px);
    PREV_PARAM(bottom_left_y, 0_px);
    PREV_PARAM(bottom_center_x, 0_px);
    PREV_PARAM(bottom_center_y, 0_px);
    PREV_PARAM(bottom_right_x, 0_px);
    PREV_PARAM(bottom_right_y, 0_px);
  }
}

void anchors::configure_anchors(
  detail::ComponentImpl&                child,
  std::optional<detail::ComponentImpl*> prev
) {
  configure_self_anchors(child);
  configure_parent_anchors(child);
  configure_prev_anchors(child, prev);
}
