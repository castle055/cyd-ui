// Copyright (c) 2025, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
export module cydui.components.configure_anchors;

export import std;

export import cydui.components.base;
export import cydui.components.anchors;

#define TO_STRING(...) #__VA_ARGS__
#define ANCHOR(PREFIX, NAME)                                                                       \
  struct NAME {                                                                                    \
    static constexpr dimension_parameter_t x{TO_STRING(PREFIX##_##NAME##_x)};                      \
    static constexpr dimension_parameter_t y{TO_STRING(PREFIX##_##NAME##_y)};                      \
  }

export {
  namespace cydui::components::anchors {
    void configure_anchors(
      std::shared_ptr<component_base_t> child, std::optional<std::shared_ptr<component_base_t>> prev
    ) {
      //! SELF
      dimension_ctx_t& ctx = *child->get_dimensional_context();
      ctx.set_parameter("self_x", child->get_dimensional_relations().x);
      ctx.set_parameter("self_y", child->get_dimensional_relations().y);
      ctx.set_parameter("self_width", child->get_dimensional_relations().width);
      ctx.set_parameter("self_height", child->get_dimensional_relations().height);
      ctx.set_parameter("self_screen_x", child->get_internal_relations().cx);
      ctx.set_parameter("self_screen_y", child->get_internal_relations().cy);
      ctx.set_parameter("self_content_width", child->get_internal_relations().cw);
      ctx.set_parameter("self_content_height", child->get_internal_relations().ch);

#define TO_STRING(...) #__VA_ARGS__
#define DIMENSIONAL_PARAM(PREFIX, NAME, ...)                                                       \
  ctx.set_parameter(TO_STRING(PREFIX##_##NAME), __VA_ARGS__)
#define PARENT_PARAM(NAME, ...) DIMENSIONAL_PARAM(parent, NAME, __VA_ARGS__ + 0_px)
#define SELF_PARAM(NAME, ...)   DIMENSIONAL_PARAM(self, NAME, __VA_ARGS__ + 0_px)

      {
        //! PARENT
        if (child->parent.has_value()) {
          auto& [cx, cy, cw, ch] = child->parent.value()->get_internal_relations();
          
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

      {
        //! PREV
#define PREV_PARAM(NAME, ...) DIMENSIONAL_PARAM(prev, NAME, __VA_ARGS__ + 0_px)

        if (prev.has_value()) {
          auto  dims = prev.value()->get_dimensional_relations();
          auto& x    = dims.x;
          auto& y    = dims.y;
          auto& w    = dims.width;
          auto& h    = dims.height;

          auto& mt = dims.margin_top;
          auto& mr = dims.margin_right;
          auto& mb = dims.margin_bottom;
          auto& ml = dims.margin_left;

          PREV_PARAM(x, x);
          PREV_PARAM(y, y);
          PREV_PARAM(width, w);
          PREV_PARAM(height, h);

          PREV_PARAM(top_left_x, x);
          PREV_PARAM(top_left_y, y);
          PREV_PARAM(top_center_x, x + ((w + ml + mr) / 2));
          PREV_PARAM(top_center_y, y);
          PREV_PARAM(top_right_x, x + w + ml + mr);
          PREV_PARAM(top_right_y, y);
          PREV_PARAM(middle_left_x, x);
          PREV_PARAM(middle_left_y, y + ((h + mt + mb) / 2));
          PREV_PARAM(center_x, x + ((w + ml + mr) / 2));
          PREV_PARAM(center_y, y + ((h + mt + mb) / 2));
          PREV_PARAM(middle_right_x, x + w + ml + mr);
          PREV_PARAM(middle_right_y, y + ((h+mt+mb) / 2));
          PREV_PARAM(bottom_left_x, x);
          PREV_PARAM(bottom_left_y, y + h + mt + mb);
          PREV_PARAM(bottom_center_x, x + ((w + ml + mr) / 2));
          PREV_PARAM(bottom_center_y, y + h + mt + mb);
          PREV_PARAM(bottom_right_x, x + w + ml + mr);
          PREV_PARAM(bottom_right_y, y + h + mt + mb);
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
    }
  } // namespace cydui::components::anchors
}
