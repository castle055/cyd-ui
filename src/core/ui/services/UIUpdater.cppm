/*! \file  UIUpdater.cppm
 *! \brief
 *!
 */
module;
#include "../../../debug/profiling/macros.h"
#define PROF_CURRENT_MODULE cydui::core::ui::services::UIUpdater


#define GET_VALUE(...)      dimensions::get_value(__VA_ARGS__).template as<dimensions::screen::pixel>().value
#define GET_IVALUE(...)                                                                                                \
  static_cast<int>(dimensions::get_value(__VA_ARGS__).template as<dimensions::screen::pixel>().value)

export module cydui.core.ui.services.UIUpdater;

export import cydui.core.ui.services.ComponentTree;
export import cydui.core.ui.services.StyleStore;
export import cydui.core.ui.services.UIRenderer;
export import cydui.core.ui.services.HoverState;

import std;
export import reflect;
import cydui.debug.profiling;

export import cydui.geometric_relations;

using namespace std::chrono_literals;

namespace cydui::detail::ui::services {
  export class UIUpdater final: public fabric::services::ServiceBase {
    PlatformController& frame_;
    ComponentTree&      tree_;
    StyleStore&         style_;
    UIRenderer&         renderer_;
    HoverState&         hover_state_;

    fabric::tasks::time_point last_update {std::chrono::milliseconds {0}};

    fabric::observable<bool> updating_ {false};
    fabric::observable<bool> pending_update_ {false};

    UIUpdater(
      PlatformController& frame,
      ComponentTree&      tree,
      StyleStore&         style,
      UIRenderer&         renderer,
      HoverState&         hover_state)
        : frame_(frame),
          tree_(tree),
          style_(style),
          renderer_(renderer),
          hover_state_(hover_state) {}

  public:
    static fabric::task<sptr> start(fabric::services::ServiceLocator& locator) {
      auto& platform    = co_await locator.require<PlatformController>();
      auto& tree        = co_await locator.require<ComponentTree>();
      auto& style_store = co_await locator.require<StyleStore>();
      auto& renderer    = co_await locator.require<UIRenderer>();
      auto& hover_state = co_await locator.require<HoverState>();

      co_return sptr {new UIUpdater(platform, tree, style_store, renderer, hover_state)};
    }

    void update_dimensions() {
      PROF_SCOPE(Update Dimensions)
      layout::compute_geometry(tree_.get_root());
    }

    fabric::task<> update_task() {
      pending_update_ = true;
      while (updating_) {
        co_await 1ms; // updating_.until_equal(false); // TODO - this segfaults immediately, the observables API seems
                      // broken
      }
      updating_       = true;
      pending_update_ = false;
      PROF_SCOPE(Update)
      FrameMarkStart("Update");

      {
        PROF_SCOPE(Update Hover)
        hover_state_.update_hover();
      }
      bool style_changed = style_.update_style(tree_.get_root());

      if (not update_ui()) {
        FrameMarkEnd("Update");
        updating_ = false;
        co_return;
      }
      style_changed |= style_.update_style(tree_.get_root());
      update_dimensions();

      {
        PROF_SCOPE(Update Hover)
        // Update Hover again in case something moved under the cursor
        if (hover_state_.update_hover() or style_changed) {
          update_ui();
          style_.update_style(tree_.get_root());
          update_dimensions();
        }
      }

      {
        PROF_SCOPE(Update Graphics)
        update_component_graphics_recurse(tree_.get_root());
      }

      co_await renderer_.render();

      FrameMarkEnd("Update");
      FrameMark;
      updating_ = false;
      co_return;
    }

    void schedule_update() {
      PROF_SCOPE(Schedule Update)
      const auto now = fabric::tasks::clock::now();
      if (last_update <= now) {
        PROF_MESSAGE("Update Scheduled");
        if ((now - last_update) >= 16ms) {
          last_update = now;
        } else {
          while (last_update <= now) {
            last_update += std::chrono::milliseconds {16};
          }
        }

        if (not pending_update_) {
          frame_.get_bus()
            ->schedule(
              [&]() -> fabric::task<> {
                co_await update_task();
                co_return;
              }(),
              last_update)
            .detach();
        }
      }
    }

    fabric::task<> reflow() {
      PROF_SCOPE(Reflow)
      update_dimensions();

      {
        PROF_SCOPE(Update Graphics)
        update_component_graphics_recurse(tree_.get_root());
      }

      co_await renderer_.render();

      FrameMark;
    }

  private:
    bool update_ui() {
      PROF_SCOPE(Update UI)
      return update_all_dirty(tree_.get_root());
    }

    bool update_all_dirty(ComponentImpl& c) {
      if (c.is_dirty()) {
        update_component(c, style_.get_style_archive());
        return true;
      } else {
        bool any = false;
        for (auto& item: c.get_children_impl())
          any = update_all_dirty(*item) || any; // ! Order here matters
        // ? update_if_dirty() needs to be called before `any` is checked.
        return any;
      }
    }

    void update_component(
      ComponentImpl&     component,
      tss::StyleArchive& style_archive) {
      PROF_SCOPE(Update Component)
      component.clear_dirty_flag();
      component.get_layer().mark_dirty();

      const BlueprintList& content_children_builder {component.get_blueprint().get_content()};

      component.get_style_stack().update_override(component.get_blueprint().get_style_override());

      BlueprintList new_children = component.get_event_dispatcher().update(style_archive, content_children_builder);

      std::list<ComponentImpl*> pending_redraw {};
      tree_.update_children(component, new_children, pending_redraw);

      layout::update_content_size(component);

      for (const auto& child: pending_redraw) {
        // Update children
        update_component(*child, style_archive);
      }
    }

  private:
    void update_component_graphics_recurse(ComponentImpl& component) {
      update_component_graphics(component);

      for (auto& child: component.get_children_impl()) {
        update_component_graphics_recurse(*child);
      }
    }

    void update_component_graphics(ComponentImpl& component) const {
      auto& layer = component.get_layer();
      if (layer.dirty) {
        PROF_SCOPE(Update Component Graphics)
        auto& at   = component.get_style();
        auto& geom = component.get_geometry();

        layer.size[0] = GET_IVALUE(geom.box_size[layout::X_AXIS]);
        layer.size[1] = GET_IVALUE(geom.box_size[layout::Y_AXIS]);

        layer.transform = AffineTransform::identity()
                            .translate(layer.size / 2)
                            .scale(vec2 {at.scale_x, at.scale_y})
                            .rotate(at.rotation)
                            .translate(layer.size / -2)
                            .translate(
                              vec2 {
                                GET_VALUE(geom.position[layout::X_AXIS]),
                                GET_VALUE(geom.position[layout::Y_AXIS]),
                              });

        layer.viewport.position[0] = GET_VALUE(geom.viewport_position[layout::X_AXIS]);
        layer.viewport.position[1] = GET_VALUE(geom.viewport_position[layout::Y_AXIS]);
        layer.viewport.size[0]     = GET_VALUE(geom.viewport_size[layout::X_AXIS]);
        layer.viewport.size[1]     = GET_VALUE(geom.viewport_size[layout::Y_AXIS]);
        layer.viewport.origin[0]   = GET_VALUE(geom.content_origin[layout::X_AXIS]);
        layer.viewport.origin[1]   = GET_VALUE(geom.content_origin[layout::Y_AXIS]);

        layer.style.animated   = component.is_animated();
        layer.style.x_overflow = at.overflow_x;
        layer.style.y_overflow = at.overflow_y;
        layer.style.position   = at.position;

        update_background(component);
        layer.elements = component.get_event_dispatcher().paint_fragment();
      }
    }

    void update_background(ComponentImpl& component) const {
      PROF_SCOPE(Update Background)
      auto& layer = component.get_layer();
      auto& style = component.get_style();
      auto& geom  = component.get_geometry();

      auto& background = layer.style.background;
      background.fill(style.background);
      background.x(dimensions::get_value(geom.background_position[layout::X_AXIS]));
      background.y(dimensions::get_value(geom.background_position[layout::Y_AXIS]));
      background.w(dimensions::get_value(geom.background_size[layout::X_AXIS]));
      background.h(dimensions::get_value(geom.background_size[layout::Y_AXIS]));
      background.border_top(style.border.top);
      background.border_right(style.border.right);
      background.border_bottom(style.border.bottom);
      background.border_left(style.border.left);
      background.border_dasharray_top(style.border_dasharray.top);
      background.border_dasharray_right(style.border_dasharray.right);
      background.border_dasharray_bottom(style.border_dasharray.bottom);
      background.border_dasharray_left(style.border_dasharray.left);
      background.border_dashoffset_top(style.border_dashoffset.top);
      background.border_dashoffset_right(style.border_dashoffset.right);
      background.border_dashoffset_bottom(style.border_dashoffset.bottom);
      background.border_dashoffset_left(style.border_dashoffset.left);
      background.border_opacity_top(style.border_opacity.top);
      background.border_opacity_right(style.border_opacity.right);
      background.border_opacity_bottom(style.border_opacity.bottom);
      background.border_opacity_left(style.border_opacity.left);
      background.border_width_top(dimensions::get_value(geom.get_border_width(edge::TOP)));
      background.border_width_right(dimensions::get_value(geom.get_border_width(edge::RIGHT)));
      background.border_width_bottom(dimensions::get_value(geom.get_border_width(edge::BOTTOM)));
      background.border_width_left(dimensions::get_value(geom.get_border_width(edge::LEFT)));

      background.border_radius_top_left(style.border_radius.top_left);
      background.border_radius_top_right(style.border_radius.top_right);
      background.border_radius_bottom_right(style.border_radius.bottom_right);
      background.border_radius_bottom_left(style.border_radius.bottom_left);
    }
  };
} // namespace cydui::detail::ui::services
