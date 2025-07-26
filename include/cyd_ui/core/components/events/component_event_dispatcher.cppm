// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include <tracy/Tracy.hpp>

export module cydui.core.event_dispatcher;

import std;

import fabric.logging;
import fabric.async;
export import fabric.wiring.signals;

export import cydui.core.event_handler;
export import cydui.core.custom_event_listener;
export import cydui.core.contexts.api;

namespace cydui::core {
  export template <typename ComponentType, typename EventHandler>
  class event_dispatcher_t final: public event_dispatcher_base_t {
  public:
    explicit event_dispatcher_t(mounted_component_t* component)
        : component_(component) {
      ZoneScopedN("event_dispatcher_t{}");

      event_handler_data_t<ComponentType> evh_data{
        component_handle_t<ComponentType>{component_},
        component_->get_state(),
        component_->get_window(),
        static_cast<ComponentType*>(component_->get_blueprint())->props,
        component_->get_style()
      };

      if constexpr (EventHandler::has_custom_state_type and EventHandler::has_custom_style_type) {
        event_handler_ = std::make_shared<EventHandler>(EventHandler{
          evh_data,
          .state = *std::dynamic_pointer_cast<state_type<ComponentType>>(component_->get_state()),
          .style =
            *static_cast<const style_type<ComponentType>*>(&component_->get_style()),
        });
      } else if constexpr (EventHandler::has_custom_state_type
                           and not EventHandler::has_custom_style_type) {
        event_handler_ = std::make_shared<EventHandler>(EventHandler{
          evh_data,
          .state = *std::dynamic_pointer_cast<state_type<ComponentType>>(component_->get_state()),
        });
      } else if constexpr (not EventHandler::has_custom_state_type
                           and EventHandler::has_custom_style_type) {
        event_handler_ = std::make_shared<EventHandler>(EventHandler{
          evh_data,
          .style =
            *static_cast<const style_type<ComponentType>*>(&component_->get_style()),
        });
      } else {
        event_handler_ = std::make_shared<EventHandler>(evh_data);
      }

      if constexpr (refl::Reflected<EventHandler>) {
        static constexpr std::size_t field_count = refl::field_count<EventHandler>;

        configure_event_handler_fields(std::make_index_sequence<field_count>{});
      }
    }

    void dispatch_mount(const content_type& content_children_builder) override {
      ZoneScopedN("Mount - Component EV");
      auto  eh   = static_cast<EventHandler*>(event_handler_.get());
      auto& geom = component_->get_geometry();
      eh->on_mount(
        geom.position[layout::X_AXIS],
        geom.position[layout::Y_AXIS],
        geom.size[layout::X_AXIS],
        geom.size[layout::Y_AXIS],
        content_children_builder
      );
    }
    void dispatch_dismount() override {
      ZoneScopedN("Mount - Component EV");
      auto  eh   = static_cast<EventHandler*>(event_handler_.get());
      auto& geom = component_->get_geometry();
      eh->on_dismount(
        geom.position[layout::X_AXIS],
        geom.position[layout::Y_AXIS],
        geom.size[layout::X_AXIS],
        geom.size[layout::Y_AXIS]
      );
    }
    content_type update(
      tss::StyleArchive&       style_archive,
      const content_type& content_children_builder
    ) override {
      ZoneScopedN("Update - Component EV");
      auto  eh   = static_cast<EventHandler*>(event_handler_.get());
      auto& geom = component_->get_geometry();
      return eh->on_redraw(
        geom.position[layout::X_AXIS],
        geom.position[layout::Y_AXIS],
        geom.viewport_size[layout::X_AXIS],
        geom.viewport_size[layout::Y_AXIS],
        content_children_builder
      );
    }
    void paint_fragment(vg::fragment_t& fragment) override {
      ZoneScopedN("Paint Fragment - Component EV");
      auto  eh   = static_cast<EventHandler*>(event_handler_.get());
      auto& geom = component_->get_geometry();
      eh->draw_fragment(
        fragment,
        dimensions::get_value(geom.position[layout::X_AXIS]),
        dimensions::get_value(geom.position[layout::Y_AXIS]),
        dimensions::get_value(geom.viewport_size[layout::X_AXIS]),
        dimensions::get_value(geom.viewport_size[layout::Y_AXIS])
      );
    }

    void dispatch_key_press(const KeyEvent& ev) override {
      ZoneScopedN("Key Press - Component EV");
      auto  eh   = static_cast<EventHandler*>(event_handler_.get());
      auto& geom = component_->get_geometry();
      eh->on_key_press(
        ev,
        dimensions::get_value(geom.position[layout::X_AXIS]),
        dimensions::get_value(geom.position[layout::Y_AXIS]),
        dimensions::get_value(geom.viewport_size[layout::X_AXIS]),
        dimensions::get_value(geom.viewport_size[layout::Y_AXIS])
      );
    }

    void dispatch_key_release(const KeyEvent& ev) override {
      ZoneScopedN("Key Release - Component EV");
      auto  eh   = static_cast<EventHandler*>(event_handler_.get());
      auto& geom = component_->get_geometry();
      eh->on_key_release(
        ev,
        dimensions::get_value(geom.position[layout::X_AXIS]),
        dimensions::get_value(geom.position[layout::Y_AXIS]),
        dimensions::get_value(geom.viewport_size[layout::X_AXIS]),
        dimensions::get_value(geom.viewport_size[layout::Y_AXIS])
      );
    }

    void dispatch_text_input(const TextInputEvent& ev) override {
      ZoneScopedN("Text Input - Component EV");
      auto  eh   = static_cast<EventHandler*>(event_handler_.get());
      auto& geom = component_->get_geometry();
      eh->on_text_input(
        ev,
        dimensions::get_value(geom.position[layout::X_AXIS]),
        dimensions::get_value(geom.position[layout::Y_AXIS]),
        dimensions::get_value(geom.viewport_size[layout::X_AXIS]),
        dimensions::get_value(geom.viewport_size[layout::Y_AXIS])
      );
    }

    void dispatch_button_press(
      const Button&           button,
      dimension_t::value_type x,
      dimension_t::value_type y
    ) override {
      ZoneScopedN("Button Press - Component EV");
      auto  eh   = static_cast<EventHandler*>(event_handler_.get());
      auto& geom = component_->get_geometry();
      eh->on_button_press(
        button,
        x,
        y,
        dimensions::get_value(geom.position[layout::X_AXIS]),
        dimensions::get_value(geom.position[layout::Y_AXIS]),
        dimensions::get_value(geom.viewport_size[layout::X_AXIS]),
        dimensions::get_value(geom.viewport_size[layout::Y_AXIS])
      );
    }

    void dispatch_button_release(
      const Button&           button,
      dimension_t::value_type x,
      dimension_t::value_type y
    ) override {
      ZoneScopedN("Button Release - Component EV");
      auto  eh   = static_cast<EventHandler*>(event_handler_.get());
      auto& geom = component_->get_geometry();
      eh->on_button_release(
        button,
        x,
        y,
        dimensions::get_value(geom.position[layout::X_AXIS]),
        dimensions::get_value(geom.position[layout::Y_AXIS]),
        dimensions::get_value(geom.viewport_size[layout::X_AXIS]),
        dimensions::get_value(geom.viewport_size[layout::Y_AXIS])
      );
    }

    void dispatch_mouse_enter(
      dimension_t::value_type x,
      dimension_t::value_type y
    ) override {
      ZoneScopedN("Mouse Enter - Component EV");
      auto  eh   = static_cast<EventHandler*>(event_handler_.get());
      auto& geom = component_->get_geometry();
      eh->on_mouse_enter(
        x,
        y,
        dimensions::get_value(geom.position[layout::X_AXIS]),
        dimensions::get_value(geom.position[layout::Y_AXIS]),
        dimensions::get_value(geom.viewport_size[layout::X_AXIS]),
        dimensions::get_value(geom.viewport_size[layout::Y_AXIS])
      );
    }

    void dispatch_mouse_exit(
      dimension_t::value_type x,
      dimension_t::value_type y
    ) override {
      ZoneScopedN("Mouse Exit - Component EV");
      auto  eh   = static_cast<EventHandler*>(event_handler_.get());
      auto& geom = component_->get_geometry();
      eh->on_mouse_exit(
        x,
        y,
        dimensions::get_value(geom.position[layout::X_AXIS]),
        dimensions::get_value(geom.position[layout::Y_AXIS]),
        dimensions::get_value(geom.viewport_size[layout::X_AXIS]),
        dimensions::get_value(geom.viewport_size[layout::Y_AXIS])
      );
    }

    void dispatch_mouse_motion(
      dimension_t::value_type x,
      dimension_t::value_type y
    ) override {
      ZoneScopedN("Mouse Motion - Component EV");
      auto  eh   = static_cast<EventHandler*>(event_handler_.get());
      auto& geom = component_->get_geometry();
      eh->on_mouse_motion(
        x,
        y,
        dimensions::get_value(geom.position[layout::X_AXIS]),
        dimensions::get_value(geom.position[layout::Y_AXIS]),
        dimensions::get_value(geom.viewport_size[layout::X_AXIS]),
        dimensions::get_value(geom.viewport_size[layout::Y_AXIS])
      );
    }

    void dispatch_scroll(
      dimension_t::value_type dx,
      dimension_t::value_type dy
    ) override {
      ZoneScopedN("Scroll - Component EV");
      auto  eh   = static_cast<EventHandler*>(event_handler_.get());
      auto& geom = component_->get_geometry();
      eh->on_scroll(
        dx,
        dy,
        dimensions::get_value(geom.position[layout::X_AXIS]),
        dimensions::get_value(geom.position[layout::Y_AXIS]),
        dimensions::get_value(geom.viewport_size[layout::X_AXIS]),
        dimensions::get_value(geom.viewport_size[layout::Y_AXIS])
      );
    }

    void dispatch_focus_changed() override {
      ZoneScopedN("Scroll - Component EV");
      auto  eh   = static_cast<EventHandler*>(event_handler_.get());
      auto& geom = component_->get_geometry();
      eh->on_focus_changed(
        dimensions::get_value(geom.position[layout::X_AXIS]),
        dimensions::get_value(geom.position[layout::Y_AXIS]),
        dimensions::get_value(geom.viewport_size[layout::X_AXIS]),
        dimensions::get_value(geom.viewport_size[layout::Y_AXIS])
      );
    }

  private:
    template <std::size_t... I>
    void configure_event_handler_fields(std::index_sequence<I...>) {
      ZoneScopedN("Configure Fields");
      (configure_event_handler_field<I>(), ...);
    }

    template <std::size_t FieldI>
    void configure_event_handler_field() {
      using field      = refl::field<EventHandler, FieldI>;
      using field_type = typename field::type;
      ZoneScopedN(field::name);
      auto* eh = static_cast<EventHandler*>(event_handler_.get());

      if constexpr (packtl::is_type<use_context, field_type>::value) {
        using context_type     = typename field_type::context_type;
        using ret_context_type = typename field_type::ret_context_type;

        use_context<ret_context_type>& ctx_ref = field::from_instance(*eh);

        auto ctx = component_->find_context<context_type>();

        if (ctx.has_value()) {
          use_context_delegate::set_context(&ctx_ref, ctx.value().get());
          use_context_delegate::set_owns_context(&ctx_ref, false);
        } else {
          use_context_delegate::set_context(&ctx_ref, new context_type{});
          use_context_delegate::set_owns_context(&ctx_ref, true);
        }

        use_context_delegate::set_window(&ctx_ref, component_->get_window());
        use_context_delegate::start_listening(&ctx_ref, [&] {
          component_->force_update();
        });
      } else if constexpr (packtl::is_type<provide_context, field_type>::value) {
        using context_type = typename field_type::context_type;

        provide_context<context_type>& ctx_ref = field::from_instance(*eh);
        provide_context_delegate::set_window(&ctx_ref, component_->get_window());

        component_->add_context<context_type>(ctx_ref);
      }
    }

  private:
    mounted_component_t*             component_{};
    std::shared_ptr<event_handler_t> event_handler_{};
  };
} // namespace cydui::components
