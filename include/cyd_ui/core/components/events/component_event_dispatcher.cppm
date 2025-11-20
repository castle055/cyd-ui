// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include <tracy/Tracy.hpp>

#include "component_event_macros.h"

export module cydui.core.event_dispatcher;

import std;

import fabric.logging;
import fabric.async;
export import fabric.wiring.signals;

export import cydui.core.event_handler;
export import cydui.core.custom_event_listener;
export import cydui.core.aspects;

namespace cydui::detail {
  export template <typename ComponentType, typename EventHandler>
  class event_dispatcher_t final: public event_dispatcher_base_t {
    event_dispatcher_base_t*         parent_;
    Component*                       component_ {};
    std::shared_ptr<event_handler_t> event_handler_ {};
    fabric::async::async_bus_t&      bus_;
    ComponentState&                  state_;
    context_store_t&                 context_store_;

  public:
    event_dispatcher_t(
      fabric::async::async_bus_t& bus,
      event_dispatcher_base_t*    parent,
      Component*                  component,
      ComponentState&             state,
      context_store_t&            context_store)
        : parent_(parent),
          component_(component),
          bus_(bus),
          state_(state),
          context_store_(context_store) {
      ZoneScopedN("event_dispatcher_t{}");

      event_handler_data_t<ComponentType> evh_data {
        TypedComponent<ComponentType> {component_},
        state_,
        bus_,
        static_cast<ComponentType*>(&component_->get_blueprint())->props,
        component_->get_style()};

      if constexpr (EventHandler::has_custom_state_type and EventHandler::has_custom_style_type) {
        event_handler_ = std::shared_ptr<EventHandler>(new EventHandler {
          evh_data,
          .state = *dynamic_cast<state_type<ComponentType>*>(&state_),
          .style = *static_cast<const style_type<ComponentType>*>(&component_->get_style()),
        });
      } else if constexpr (EventHandler::has_custom_state_type and not EventHandler::has_custom_style_type) {
        event_handler_ = std::shared_ptr<EventHandler>(new EventHandler {
          evh_data,
          .state = *dynamic_cast<state_type<ComponentType>*>(&state_),
        });
      } else if constexpr (not EventHandler::has_custom_state_type and EventHandler::has_custom_style_type) {
        event_handler_ = std::shared_ptr<EventHandler>(new EventHandler {
          evh_data,
          .style = *static_cast<const style_type<ComponentType>*>(&component_->get_style()),
        });
      } else {
        event_handler_ = std::make_shared<EventHandler>(evh_data);
      }

      configure_fields();
    }

    std::shared_ptr<void> get_event_handler() override {
      return event_handler_;
    }

    void dispatch_mount(const BlueprintList& content_children_builder) override {
      ZoneScopedN("Mount - Component EV");
      auto  eh   = static_cast<EventHandler*>(event_handler_.get());
      auto& geom = component_->get_geometry();
      eh->on_mount(CYDUI_INTERNAL_EV_DIM_ARGS, content_children_builder);
    }

    void dispatch_dismount() override {
      ZoneScopedN("Mount - Component EV");
      auto  eh   = static_cast<EventHandler*>(event_handler_.get());
      auto& geom = component_->get_geometry();
      eh->on_dismount(CYDUI_INTERNAL_EV_DIM_ARGS);
    }

    BlueprintList update(
      tss::StyleArchive&   style_archive,
      const BlueprintList& content_children_builder) override {
      ZoneScopedN("Update - Component EV");
      auto  eh   = static_cast<EventHandler*>(event_handler_.get());
      auto& geom = component_->get_geometry();
      update_fields();
      return eh->on_redraw(CYDUI_INTERNAL_EV_DIM_ARGS, content_children_builder);
    }

    ElementVector paint_fragment() override {
      ZoneScopedN("Paint Fragment - Component EV");
      auto  eh   = static_cast<EventHandler*>(event_handler_.get());
      auto& geom = component_->get_geometry();
      return eh->draw_fragment(CYDUI_INTERNAL_EV_VAL_ARGS);
    }

    void dispatch_key_press(const KeyEvent& ev) override {
      ZoneScopedN("Key Press - Component EV");
      auto  eh        = static_cast<EventHandler*>(event_handler_.get());
      auto& geom      = component_->get_geometry();
      bool  propagate = false;

      eh->on_key_press(ev, CYDUI_INTERNAL_EV_VAL_ARGS, propagate);

      if (propagate) {
        if (nullptr != parent_) parent_->dispatch_key_press(ev);
      }
    }

    void dispatch_key_release(const KeyEvent& ev) override {
      ZoneScopedN("Key Release - Component EV");
      auto  eh        = static_cast<EventHandler*>(event_handler_.get());
      auto& geom      = component_->get_geometry();
      bool  propagate = false;

      eh->on_key_release(ev, CYDUI_INTERNAL_EV_VAL_ARGS, propagate);

      if (propagate) {
        if (nullptr != parent_) parent_->dispatch_key_release(ev);
      }
    }

    void dispatch_text_input(const TextInputEvent& ev) override {
      ZoneScopedN("Text Input - Component EV");
      auto  eh   = static_cast<EventHandler*>(event_handler_.get());
      auto& geom = component_->get_geometry();
      eh->on_text_input(ev, CYDUI_INTERNAL_EV_VAL_ARGS);
    }

    void dispatch_button_press(
      const Button&           button,
      dimension_t::value_type x,
      dimension_t::value_type y) override {
      ZoneScopedN("Button Press - Component EV");
      auto  eh        = static_cast<EventHandler*>(event_handler_.get());
      auto& geom      = component_->get_geometry();
      bool  propagate = false;

      eh->on_button_press(button, x, y, CYDUI_INTERNAL_EV_VAL_ARGS, propagate);

      if (propagate) {
        if (nullptr != parent_)
          parent_->dispatch_button_press(
            button,
            x + dimensions::get_value(geom.position[layout::X_AXIS]),
            y + dimensions::get_value(geom.position[layout::Y_AXIS]));
      }
    }

    void dispatch_button_release(
      const Button&           button,
      dimension_t::value_type x,
      dimension_t::value_type y) override {
      ZoneScopedN("Button Release - Component EV");
      auto  eh        = static_cast<EventHandler*>(event_handler_.get());
      auto& geom      = component_->get_geometry();
      bool  propagate = false;

      eh->on_button_release(button, x, y, CYDUI_INTERNAL_EV_VAL_ARGS, propagate);

      if (propagate) {
        if (nullptr != parent_)
          parent_->dispatch_button_release(
            button,
            x + dimensions::get_value(geom.position[layout::X_AXIS]),
            y + dimensions::get_value(geom.position[layout::Y_AXIS]));
      }
    }

    void dispatch_mouse_enter(
      dimension_t::value_type x,
      dimension_t::value_type y) override {
      ZoneScopedN("Mouse Enter - Component EV");
      auto  eh   = static_cast<EventHandler*>(event_handler_.get());
      auto& geom = component_->get_geometry();
      eh->on_mouse_enter(x, y, CYDUI_INTERNAL_EV_VAL_ARGS);
    }

    void dispatch_mouse_exit(
      dimension_t::value_type x,
      dimension_t::value_type y) override {
      ZoneScopedN("Mouse Exit - Component EV");
      auto  eh   = static_cast<EventHandler*>(event_handler_.get());
      auto& geom = component_->get_geometry();
      eh->on_mouse_exit(x, y, CYDUI_INTERNAL_EV_VAL_ARGS);
    }

    void dispatch_mouse_motion(
      dimension_t::value_type x,
      dimension_t::value_type y) override {
      ZoneScopedN("Mouse Motion - Component EV");
      auto  eh        = static_cast<EventHandler*>(event_handler_.get());
      auto& geom      = component_->get_geometry();
      bool  propagate = false;

      eh->on_mouse_motion(x, y, CYDUI_INTERNAL_EV_VAL_ARGS, propagate);

      if (propagate) {
        if (nullptr != parent_)
          parent_->dispatch_mouse_motion(
            x + dimensions::get_value(geom.position[layout::X_AXIS]),
            y + dimensions::get_value(geom.position[layout::Y_AXIS]));
      }
    }

    void dispatch_scroll(
      dimension_t::value_type dx,
      dimension_t::value_type dy) override {
      ZoneScopedN("Scroll - Component EV");
      auto  eh        = static_cast<EventHandler*>(event_handler_.get());
      auto& geom      = component_->get_geometry();
      bool  propagate = false;

      eh->on_scroll(dx, dy, CYDUI_INTERNAL_EV_VAL_ARGS, propagate);

      if (propagate) {
        if (nullptr != parent_) parent_->dispatch_scroll(dx, dy);
      }
    }

    void dispatch_focus_changed() override {
      ZoneScopedN("Scroll - Component EV");
      auto  eh   = static_cast<EventHandler*>(event_handler_.get());
      auto& geom = component_->get_geometry();
      eh->on_focus_changed(CYDUI_INTERNAL_EV_VAL_ARGS);
    }

  private:
    void configure_fields() {
      if constexpr (refl::Reflected<EventHandler>) {
        static constexpr std::size_t field_count = refl::field_count<EventHandler>;
        if constexpr (field_count > 0) {
          [&]<std::size_t... I>(std::index_sequence<I...>) {
            ZoneScopedN("Configure Fields");
            (configure_field_impl<I>(), ...);
          }(std::make_index_sequence<field_count> {});
        }
      }
    }

    template <std::size_t FieldI>
    void configure_field_impl() {
      using field      = refl::field<EventHandler, FieldI>;
      using field_type = typename field::type;
      ZoneScopedN(field::name);
      auto* eh = static_cast<EventHandler*>(event_handler_.get());

      if constexpr (IsComponentAspect<field_type>) {
        field_type&      ref         = field::from_instance(*eh);
        ComponentAspect& aspect_base = get_aspect_base(ref);
        aspect_base.mount(bus_, component_, context_store_);
      }
    }

    void update_fields() {
      if constexpr (refl::Reflected<EventHandler>) {
        static constexpr std::size_t field_count = refl::field_count<EventHandler>;
        if constexpr (field_count > 0) {
          [&]<std::size_t... I>(std::index_sequence<I...>) {
            ZoneScopedN("Update Fields");
            (update_field_impl<I>(), ...);
          }(std::make_index_sequence<field_count> {});
        }
      }
    }

    template <std::size_t FieldI>
    void update_field_impl() {
      using field      = refl::field<EventHandler, FieldI>;
      using field_type = typename field::type;
      ZoneScopedN(field::name);
      auto* eh = static_cast<EventHandler*>(event_handler_.get());

      if constexpr (IsComponentAspect<field_type>) {
        field_type&      ref         = field::from_instance(*eh);
        ComponentAspect& aspect_base = get_aspect_base(ref);
        aspect_base.update();
      }
    }
  };
} // namespace cydui::detail
