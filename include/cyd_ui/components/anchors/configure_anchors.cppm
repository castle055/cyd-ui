// Copyright (c) 2025, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
export module cydui.components.configure_anchors;

export import std;

export import cydui.components.base;
export import cydui.components.anchors;

#define TO_STRING(...) #__VA_ARGS__
#define ANCHOR(PREFIX, NAME) \
  struct NAME { \
    static constexpr dimension_parameter_t x{TO_STRING(PREFIX##_##NAME##_x)}; \
    static constexpr dimension_parameter_t y{TO_STRING(PREFIX##_##NAME##_y)}; \
  }

export
{
  namespace cydui::components::anchors {
    void configure_anchors(
      std::shared_ptr<component_base_t> child,
      std::optional<std::shared_ptr<component_base_t>> prev
    ) {
      //! SELF
      dimension_ctx_t &ctx = *child->get_dimensional_context();
      ctx.set_parameter("self_x", [child] {
        return child->get_dimensional_relations().x;
      });
      ctx.set_parameter("self_y", [child] {
        return child->get_dimensional_relations().y;
      });
      ctx.set_parameter("self_width", [child] {
        return child->get_dimensional_relations().width;
      });
      ctx.set_parameter("self_height", [child] {
        return child->get_dimensional_relations().height;
      });
      ctx.set_parameter("self_screen_x", [child] {
        return child->get_internal_relations().cx;
      });
      ctx.set_parameter("self_screen_y", [child] {
        return child->get_internal_relations().cy;
      });
      ctx.set_parameter("self_content_width", [child] {
        return child->get_internal_relations().cw;
      });
      ctx.set_parameter("self_content_height", [child] {
        return child->get_internal_relations().ch;
      });

#define TO_STRING(...) #__VA_ARGS__
#define DIMENSIONAL_PARAM(PREFIX, NAME, ...) \
        ctx.set_parameter(TO_STRING(PREFIX##_##NAME), __VA_ARGS__)
#define PARENT_PARAM(NAME, ...) DIMENSIONAL_PARAM(parent, NAME, [child] { \
          auto& [cx, cy, cw, ch] = child->parent.value()->get_internal_relations(); \
          return dimension_t {__VA_ARGS__}; \
        })
#define SELF_PARAM(NAME, ...) DIMENSIONAL_PARAM(self, NAME, __VA_ARGS__)

      //! PARENT
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

      //! PREV
#define PREV_PARAM(NAME, ...)                                                                      \
  DIMENSIONAL_PARAM(prev, NAME, [=] {                                                              \
    if (prev.has_value()) {                                                                        \
      auto  dims = prev.value()->get_dimensional_relations();                                      \
      auto& x    = dims.x;                                                                         \
      auto& y    = dims.y;                                                                         \
      auto& w    = dims.width;                                                                     \
      auto& h    = dims.height;                                                                    \
      return dimension_t{__VA_ARGS__};                                                             \
    }                                                                                              \
    return dimension_t{0_px};                                                                      \
  })

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
    }
  } // namespace cydui::components
}
