/*! \file  UI.cxx
 *! \brief
 *!
 */

module;
#include <tracy/Tracy.hpp>
#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>


module cydui.ui_handle;

import std;
import fabric.logging;
import fabric.profiling;

import cydui.application;
import cydui.dimensions;
import cydui.styling.lang;
import cydui.core.blueprint;
import cydui.geometric_relations;

import cydui.core.frame_controller;
import cydui.core.style;
import cydui.core.focus_state;
import cydui.core.hover_state;
import cydui.core.compositor;
import cydui.core.renderer;
import cydui.core.updater;
import cydui.core.ev_dispatch;

using namespace cydui;

namespace cydui::core {
  class UI_impl {
    FrameController frame_;
    StyleStore      style_{};
    FocusState      focus_state_{frame_, style_};
    ComponentTree   tree_{style_, focus_state_};
    UICompositor    compositor_{frame_, tree_};
    HoverState      hover_state_{style_, tree_};
    UIRenderer      renderer_{frame_, tree_, compositor_};
    UIUpdater       updater_{frame_, tree_, style_, renderer_, hover_state_};

    event_dispatcher event_dispatcher_{frame_, tree_, style_, hover_state_, focus_state_, updater_};

  private:
    UI_impl(
      const backends::frame_base::sptr& frame,
      mounted_component_t::uptr         root,
      const UI_options&                 options
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
      geom.set_position_absolute(layout::X_AXIS);
      geom.set_position_absolute(layout::Y_AXIS);
      geom.set_sizing_mode(layout::X_AXIS, layout::component_sizing::FIXED);
      geom.set_sizing_mode(layout::Y_AXIS, layout::component_sizing::FIXED);
      tree_.set_root_size(double(w), double(h));

      frame_.get_bus()->get_spawn_context()->set_resource<animations::AnimationSystem>(
        std::make_shared<animations::AnimationSystem>(frame_.get_bus())
      );

      animations::AnimationSystem& anim_system =
        *frame_.get_bus()->get_spawn_context()->get_resource<animations::AnimationSystem>();

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
      const backends::frame_base::sptr& frame,
      core::mounted_component_t::uptr   root,
      const UI_options&                 options
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
} // namespace cydui::core

std::shared_ptr<core::UI_impl> UI::make_impl(
  const backends::frame_base::sptr& frame,
  core::mounted_component_t::uptr   root,
  const UI_options&                 options
) {
  return core::UI_impl::make(frame, std::move(root), options);
}

void UI::attach_stylesheet(const tss::StyleSheet::sptr& style_sheet) const {
  impl_->get_style().attach_stylesheet(style_sheet);
}

void UI::attach_stylesheet(const std::filesystem::path& style_sheet) const {
  attach_stylesheet(tss::StyleSheet::parse(style_sheet));
}

void UI::add_style(const std::string& style_string) const {
  impl_->attach_stylesheet(tss::StyleSheet::parse(style_string));
}
