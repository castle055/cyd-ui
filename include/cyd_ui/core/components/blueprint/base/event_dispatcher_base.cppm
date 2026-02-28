// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

export module cydui.core.blueprint.base:event_dispatcher;
import :content;

import std;
import fabric.logging;

export import cydui.event_types;
export import cydui.styling.lang;
export import cydui.elements;

export namespace cydui {
  class Blueprint;
}
export namespace cydui::detail {
  class event_dispatcher_base_t {
  public:
    virtual ~event_dispatcher_base_t() = default;

    virtual std::shared_ptr<void> get_event_handler() = 0;

    virtual BlueprintList update(
      tss::StyleArchive&  style_archive,
      const BlueprintList& content_children_builder
    ) = 0;

    virtual void layout() = 0;

    virtual ElementVector paint_fragment() = 0;

    virtual void dispatch_mount(const BlueprintList& content_children_builder) = 0;
    virtual void dispatch_dismount()                                          = 0;
    virtual void dispatch_key_press(const KeyEvent& ev)                       = 0;
    virtual void dispatch_key_release(const KeyEvent& ev)                     = 0;
    virtual void dispatch_text_input(const TextInputEvent& ev)                = 0;
    virtual void dispatch_button_press(
      const Button&           ev,
      dimension_t::value_type x,
      dimension_t::value_type y
    ) = 0;
    virtual void dispatch_button_release(
      const Button&           ev,
      dimension_t::value_type x,
      dimension_t::value_type y
    ) = 0;
    virtual void dispatch_scroll(
      dimension_t::value_type dx,
      dimension_t::value_type dy
    ) = 0;
    virtual void dispatch_mouse_enter(
      dimension_t::value_type x,
      dimension_t::value_type y
    ) = 0;
    virtual void dispatch_mouse_exit(
      dimension_t::value_type x,
      dimension_t::value_type y
    ) = 0;
    virtual void dispatch_mouse_motion(
      dimension_t::value_type x,
      dimension_t::value_type y
    )                                     = 0;
    virtual void dispatch_focus_changed() = 0;
  };
} // namespace cydui::core
