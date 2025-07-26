/*! \file  UI.cppm
 *! \brief
 *!
 */

module;
#include <tracy/Tracy.hpp>
#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>


export module cydui.ui_handle;

import std;
import reflect;
import fabric.logging;
import fabric.profiling;

export import cydui.application;
export import cydui.dimensions;
export import cydui.styling.lang;

export import cydui.core.blueprint;
export import cydui.ui_options;

import cydui.geometric_relations;

export namespace cydui::core {
  class UI_impl;
} // namespace cydui::core

namespace cydui {
  export class UI {
    std::shared_ptr<core::UI_impl> impl_;

  public:
    template <core::ComponentBlueprint C>
    UI(
      const backends::frame_base::sptr& frame,
      const C&                          root_component,
      const UI_options&                 options
    ) {
      ZoneScopedN("UI::make");
      std::unique_ptr<C>              root_blueprint = std::make_unique<C>(root_component);
      core::mounted_component_t::uptr root =
        core::mounted_component_t::make(frame, std::move(root_blueprint));

      impl_ = make_impl(frame, std::move(root), options);
    }

    void attach_stylesheet(const tss::StyleSheet::sptr& style_sheet) const;

    void attach_stylesheet(const std::filesystem::path& style_sheet) const;

    void add_style(const std::string& style_string) const;

  private:
    static std::shared_ptr<core::UI_impl> make_impl(
      const backends::frame_base::sptr& frame,
      core::mounted_component_t::uptr   root,
      const UI_options&                 options
    );
  };

  export template <core::ComponentBlueprint C>
  UI make_ui(
    const backends::frame_base::sptr& frame,
    const C&                          root_component,
    UI_options                        options = {}
  ) {
    return UI{frame, root_component, options};
  }
} // namespace cydui
