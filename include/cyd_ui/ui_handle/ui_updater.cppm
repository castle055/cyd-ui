/*! \file  ui_updater.cppm
 *! \brief
 *!
 */
module;
#include "cyd_ui/debug/profiling/macros.h"
#define PROF_CURRENT_MODULE cydui::layout::ui_updater


#define GET_VALUE(...)                                                                             \
  dimensions::get_value(__VA_ARGS__).template as<dimensions::screen::pixel>().value
#define GET_IVALUE(...)                                                                            \
  static_cast<int>(                                                                                \
    dimensions::get_value(__VA_ARGS__).template as<dimensions::screen::pixel>().value              \
  )

export module cydui.layout.ui_updater;
export import cydui.layout.ui_tree;
export import cydui.layout.ui_style;
export import cydui.layout.ui_renderer;
export import cydui.layout.hover_state;

import std;
export import reflect;
import cydui.debug.profiling;

export import cydui.components.mounted;
export import cydui.geometric_relations;

namespace cydui {
  export class ui_updater {
    ui_frame&    frame_;
    ui_tree&     tree_;
    ui_style&    style_;
    ui_renderer& renderer_;
    hover_state& hover_state_;

    fabric::tasks::time_point last_update{std::chrono::milliseconds{0}};

  public:
    ui_updater(
      ui_frame&    frame,
      ui_tree&     tree,
      ui_style&    style,
      ui_renderer& renderer,
      hover_state& hover_state
    )
        : frame_(frame),
          tree_(tree),
          style_(style),
          renderer_(renderer),
          hover_state_(hover_state) {}

    void update_dimensions() {
      PROF_SCOPE(Update Dimensions)
      geometry::compute_geometry(*tree_.root);
    }

    fabric::task<> update_task() {
      PROF_SCOPE(Update)
      FrameMarkStart("Update");

      {
        PROF_SCOPE(Update Hover)
        hover_state_.update_hover();
      }

      if (not update_ui()) {
        FrameMarkEnd("Update");
        co_return;
      }
      style_.update_style(*tree_.root);
      update_dimensions();

      {
        PROF_SCOPE(Update Hover)
        // Update Hover again in case something moved under the cursor
        if (hover_state_.update_hover()) {
          update_ui();
          style_.update_style(*tree_.root);
          update_dimensions();
        }
      }

      {
        PROF_SCOPE(Update Graphics)
        update_component_graphics_recurse(*tree_.root);
      }

      renderer_.render();

      FrameMarkEnd("Update");
      FrameMark;
      co_return;
    }

    void schedule_update() {
      PROF_SCOPE(Schedule Update)
      const auto now = fabric::tasks::clock::now();
      if (last_update <= now) {
        PROF_MESSAGE("Update Scheduled");
        while (last_update <= now) {
          last_update += std::chrono::milliseconds{16};
        }

        frame_.get_bus()->schedule(last_update, [&]() -> fabric::task<> {
          co_await update_task();
          co_return;
        });
      }
    }

    void reflow() {
      PROF_SCOPE(Reflow)
      update_dimensions();

      {
        PROF_SCOPE(Update Graphics)
        update_component_graphics_recurse(*tree_.root);
      }

      renderer_.render();

      FrameMark;
    }

  private:
    bool update_ui() {
      PROF_SCOPE(Update UI)
      return update_all_dirty(*tree_.root);
    }

    bool update_all_dirty(components::mounted_component_t& c) {
      if (c.is_dirty()) {
        update_component(c, style_.get_style_archive());
        return true;
      } else {
        bool any = false;
        for (auto& item: c.get_children())
          any = update_all_dirty(*item) || any; // ! Order here matters
        // ? update_if_dirty() needs to be called before `any` is checked.
        return any;
      }
    }

    void update_component(
      components::mounted_component_t& component,
      StyleArchive&                    style_archive
    ) {
      PROF_SCOPE(Update Component)
      component.clear_dirty_flag();
      component.get_compositing_node().queue_graphics_update();

      const components::content_type& content_children_builder{
        component.get_blueprint()->get_content()
      };

      component.get_style_stack().update_override(component.get_blueprint()->get_style_override());

      components::content_type new_children =
        component.get_event_dispatcher().update(style_archive, content_children_builder);

      std::list<components::mounted_component_t*> pending_redraw{};
      tree_.update_children(component, new_children, pending_redraw);

      geometry::update_content_size(component);

      for (const auto& child: pending_redraw) {
        // Update children
        update_component(*child, style_archive);
      }
    }

  private:
    void update_component_graphics_recurse(components::mounted_component_t& component) {
      update_component_graphics(component);

      for (auto& child: component.get_children()) {
        update_component_graphics_recurse(*child);
      }
    }

    void update_component_graphics(components::mounted_component_t& component) const {
      auto& node = component.get_compositing_node();

      if (node.graphics_dirty_) {
        PROF_SCOPE(Update Component Graphics)
        auto& at   = component.get_style();
        auto& geom = component.get_geometry();

        node.update_operation({
          .x          = GET_IVALUE(geom.position[geometry::X_AXIS]),
          .y          = GET_IVALUE(geom.position[geometry::Y_AXIS]),
          .orig_x     = GET_IVALUE(geom.content_origin[geometry::X_AXIS]),
          .orig_y     = GET_IVALUE(geom.content_origin[geometry::Y_AXIS]),
          .w          = GET_IVALUE(geom.box_size[geometry::X_AXIS]),
          .h          = GET_IVALUE(geom.box_size[geometry::Y_AXIS]),
          .vx         = GET_IVALUE(geom.viewport_position[geometry::X_AXIS]),
          .vy         = GET_IVALUE(geom.viewport_position[geometry::Y_AXIS]),
          .vw         = GET_IVALUE(geom.viewport_size[geometry::X_AXIS]),
          .vh         = GET_IVALUE(geom.viewport_size[geometry::Y_AXIS]),
          .rot        = at.rotation.value_as_base_unit(), // dim->rot.val(),
          .scale_x    = 1.0,                              // dim->scale_x.val(),
          .scale_y    = 1.0,                              // dim->scale_y.val(),
          .animated   = component.is_animated(),
          .x_overflow = at.overflow_x,
          .y_overflow = at.overflow_y,
        });
        node.mark_flattening_target_dirty();
        define_background(component, node.background_graphics);
        define_fragment(component, node.graphics);
      }
    }

    void define_background(
      components::mounted_component_t& component,
      vg::fragment_t&                  background
    ) const {
      PROF_SCOPE(Define Background)
      background.clear();

      auto& at                 = component.get_style();
      auto& geom               = component.get_geometry();
      int   half_top_border    = GET_IVALUE(geom.get_border_width(edge::TOP)) >> 1;
      int   half_bottom_border = (GET_IVALUE(geom.get_border_width(edge::BOTTOM)) >> 1)
                               + (GET_IVALUE(geom.get_border_width(edge::BOTTOM)) & 1);
      int half_left_border  = GET_IVALUE(geom.get_border_width(edge::LEFT)) >> 1;
      int half_right_border = (GET_IVALUE(geom.get_border_width(edge::RIGHT)) >> 1)
                              + (GET_IVALUE(geom.get_border_width(edge::RIGHT)) & 1);

      // The four border corners, in reading order (left -> right, top ->bottom)
      int x1 = (half_left_border);
      int y1 = (half_top_border);

      int x2 =
        x1 + GET_IVALUE(geom.box_size[geometry::X_AXIS]) - (half_left_border) - (half_right_border);
      int y2 = y1;

      int x3 = x1;
      int y3 =
        y1 + GET_IVALUE(geom.box_size[geometry::Y_AXIS]) - (half_top_border) - (half_bottom_border);

      int x4 = x2;
      int y4 = y3;

      background.draw<vg::rectangle>()
        .x(dimensions::get_value(geom.background_position[geometry::X_AXIS]))
        .y(dimensions::get_value(geom.background_position[geometry::Y_AXIS]))
        .w(dimensions::get_value(geom.background_size[geometry::X_AXIS]))
        .h(dimensions::get_value(geom.background_size[geometry::Y_AXIS]))
        .fill(at.background);
      background.draw<vg::line>()
        .x1(x1 - half_left_border)
        .y1(y1)
        .x2(x2 + half_right_border)
        .y2(y2)
        .stroke(at.border.top)
        .stroke_width(dimensions::get_value(geom.get_border_width(edge::TOP)))
        .stroke_dasharray(at.border_dasharray.top);
      background.draw<vg::line>()
        .x1(x4 + half_right_border)
        .y1(y4)
        .x2(x3 - half_left_border)
        .y2(y3)
        .stroke(at.border.bottom)
        .stroke_width(dimensions::get_value(geom.get_border_width(edge::BOTTOM)))
        .stroke_dasharray(at.border_dasharray.bottom);
      background.draw<vg::line>()
        .x1(x3)
        .y1(y3 + half_bottom_border)
        .x2(x1)
        .y2(y1 - half_top_border)
        .stroke(at.border.left)
        .stroke_width(dimensions::get_value(geom.get_border_width(edge::LEFT)))
        .stroke_dasharray(at.border_dasharray.left);
      background.draw<vg::line>()
        .x1(x2)
        .y1(y2 - half_top_border)
        .x2(x4)
        .y2(y4 + half_bottom_border)
        .stroke(at.border.right)
        .stroke_width(dimensions::get_value(geom.get_border_width(edge::RIGHT)))
        .stroke_dasharray(at.border_dasharray.right);
    }

    void define_fragment(
      components::mounted_component_t& component,
      vg::fragment_t&                  fragment
    ) const {
      PROF_SCOPE(Define Fragment)
      fragment.clear();
      component.get_event_dispatcher().paint_fragment(fragment);
    }
  };
} // namespace cydui
