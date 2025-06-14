// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

export module cydui.components.blueprint.base:event_dispatcher;

import std;

import fabric.logging;

export import cydui.events;
export import cydui.styling.lang;
export import cydui.graphics.vector;

export namespace cydui::components {
  class blueprint_base_t;

  class content_element final: public std::unique_ptr<blueprint_base_t> {
  public:
    //! @note We do NOT want this to be an explicit constructor.
    template <typename BlueprintType>
    content_element(std::unique_ptr<BlueprintType> blueprint)
        : std::unique_ptr<blueprint_base_t>(std::move(blueprint)) {}

    //! @note We do NOT want this to be an explicit constructor.
    template <typename BlueprintType>
    content_element(const BlueprintType& blueprint)
        : std::unique_ptr<blueprint_base_t>(new BlueprintType{blueprint}) {}
  };

  class content_type;

  class event_dispatcher_base_t {
  public:
    virtual ~event_dispatcher_base_t() = default;

    virtual content_type update(
      StyleArchive&       style_archive,
      const content_type& content_children_builder
    ) = 0;

    virtual void paint_fragment(vg::fragment_t& fragment) = 0;

    virtual void dispatch_mount(const content_type& content_children_builder) = 0;
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
} // namespace cydui::components
