/*! \file  layout.cppm
 *! \brief
 *!
 */

module;
#include <tracy/Tracy.hpp>
#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>


export module cydui.ui_handle;

import std;
import fabric.logging;
import fabric.profiling;

export import cydui.application;
export import cydui.dimensions;
export import cydui.styling.lang;

export import cydui.components.blueprint;

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
export import cydui.ui_impl;

namespace cydui {
  export class UI {
    UI_impl::sptr impl_;

  public:
    template <components::ComponentBlueprint C>
    UI(
      const backends::frame_base::sptr& frame,
      const C&                          root_component,
      const UI_options& options
    ) {
      ZoneScopedN("UI::make");
      std::unique_ptr<C>                    root_blueprint = std::make_unique<C>(root_component);
      components::mounted_component_t::uptr root =
        components::mounted_component_t::make(frame, std::move(root_blueprint));

      impl_ = UI_impl::make(frame, std::move(root), options);
    }

    void attach_stylesheet(const tss::StyleSheet::sptr& style_sheet) const {
      impl_->get_style().attach_stylesheet(style_sheet);
    }

    void attach_stylesheet(const std::filesystem::path& style_sheet) const {
      impl_->get_style().attach_stylesheet(tss::StyleSheet::parse(style_sheet));
    }

    void add_style(const std::string& style_string) const {
      impl_->attach_stylesheet(tss::StyleSheet::parse(style_string));
    }
  };

  export template <components::ComponentBlueprint C>
  UI make_ui(
    const backends::frame_base::sptr& frame,
    const C&                          root_component,
    UI_options options = {}
  ) {
    return UI{frame, root_component, options};
  }
} // namespace cydui
