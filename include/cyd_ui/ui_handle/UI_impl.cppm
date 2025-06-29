/*! \file  layout.cppm
 *! \brief
 *!
 */

module;
#include <tracy/Tracy.hpp>
#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>


export module cydui.ui_impl;

import std;
import fabric.logging;
import fabric.profiling;

export import cydui.application;
export import cydui.dimensions;
export import cydui.styling.lang;

export import cydui.components.mounted;

import cydui.geometric_relations;

export import cydui.layout.ui_frame;
export import cydui.layout.ui_tree;
export import cydui.layout.ui_compositor;
export import cydui.layout.ui_style;
export import cydui.layout.hover_state;
export import cydui.layout.focus_state;
export import cydui.layout.ui_renderer;
export import cydui.layout.ui_updater;
export import cydui.layout.ev_dispatch;

export import cydui.animations;

namespace cydui {
  export struct UI_options {
    std::vector<tss::StyleSheet::sptr> stylesheets;

    void attach_stylesheet(const std::filesystem::path& style_sheet) {
      stylesheets.push_back(tss::StyleSheet::parse(style_sheet));
    }

    void attach_style(const std::string& style_str) {
      stylesheets.push_back(tss::StyleSheet::parse(style_str));
    }
  };

  export class UI_impl {
    ui_frame      frame_;
    ui_style      style_{};
    focus_state   focus_state_{frame_, style_};
    ui_tree       tree_{style_, focus_state_};
    ui_compositor compositor_{frame_, tree_};
    hover_state   hover_state_{style_, tree_};
    ui_renderer   renderer_{frame_, tree_, compositor_};
    ui_updater    updater_{frame_, tree_, style_, renderer_, hover_state_};

    event_dispatcher event_dispatcher_{frame_, tree_, style_, hover_state_, focus_state_, updater_};

  private:
    UI_impl(
      const backends::frame_base::sptr&     frame,
      components::mounted_component_t::uptr root,
      const UI_options&                     options
    )
        : frame_(frame) {
      ZoneScopedN("UI_impl{}");
      for (const auto& stylesheet: options.stylesheets) {
        style_.attach_stylesheet(stylesheet);
      }

      tree_.root_state = root->get_state();
      tree_.root       = std::move(root);

      /// Configure root component
      auto [w, h] = frame_.get_size();

      auto& geom = tree_.root->get_geometry();
      geom.set_position_absolute(geometry::X_AXIS);
      geom.set_position_absolute(geometry::Y_AXIS);
      geom.set_sizing_mode(geometry::X_AXIS, geometry::component_sizing::FIXED);
      geom.set_sizing_mode(geometry::Y_AXIS, geometry::component_sizing::FIXED);
      tree_.set_root_size(double(w), double(h));

      frame_.get_bus()->get_spawn_context()->set_resource<AnimationSystem>(
        std::make_shared<AnimationSystem>(frame_.get_bus())
      );

      AnimationSystem& anim_system =
        *frame_.get_bus()->get_spawn_context()->get_resource<AnimationSystem>();

      anim_system.s_render_all.connect([&]() { renderer_.render(); });
      anim_system.s_compose_all.connect([&]() {
        Application::run([&]() { compositor_.compose(); });
      });

      style_.compile_rules(*tree_.root);
      style_.update_style(*tree_.root);
      updater_.schedule_update();

      event_dispatcher_.start_listeners();
    }

  public:
    using sptr = std::shared_ptr<UI_impl>;

    static sptr make(
      const backends::frame_base::sptr&     frame,
      components::mounted_component_t::uptr root,
      const UI_options&                     options
    ) {
      return frame->get_executor()
        ->schedule([&]() -> fabric::task<std::shared_ptr<UI_impl>> {
          co_return std::shared_ptr<UI_impl>(new UI_impl(frame, std::move(root), options));
        })
        .get();
    }

    void attach_stylesheet(const tss::StyleSheet::sptr& style_sheet) {
      frame_.get_bus()->schedule(
        [&](tss::StyleSheet::sptr style_sheet_) -> fabric::task<> {
          style_.attach_stylesheet(style_sheet_);
          style_.compile_rules_recurse(*tree_.root);
          updater_.schedule_update();
          co_return;
        },
        style_sheet
      );
    }

    // drag_n_drop::dragging_context_t dragging_context {};

    auto& get_style() {
      return style_;
    }
  };
} // namespace cydui
