// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include <tracy/Tracy.hpp>

export module cydui.components.event_dispatcher;

import std;

import fabric.logging;
import fabric.memory.lazy_alloc;
import fabric.async;
export import fabric.wiring.signals;

export import cydui.components.event_handler;

export using children_list = std::vector<cydui::components::component_holder_t>;

namespace cydui::components {
  export template <typename ComponentType, typename EventHandler>
  class event_dispatcher_t final: public event_dispatcher_base_t {
  public:
    explicit event_dispatcher_t(component_base_t *component): component_(component) {
      ZoneScopedN("event_dispatcher_t");
      auto &style = *static_cast<typename ComponentType::style_t *>(&component_->get_style_data().as_base());
      event_handler_ = std::make_shared<EventHandler>(event_handler_data_t<ComponentType> {
        *static_cast<ComponentType*>(component_),
        component_->children,
        std::dynamic_pointer_cast<typename ComponentType::state_t>(component_->state()),
        (std::dynamic_pointer_cast<typename ComponentType::state_t>(component_->state()))->window,
        static_cast<ComponentType*>(component_)->props,
        *static_cast<attrs_component<ComponentType>*>(static_cast<ComponentType*>(component_)),
        style
      });

      if constexpr (refl::Reflected<EventHandler>) {
        static constexpr std::size_t field_count = refl::field_count<EventHandler>;

        configure_event_handler_fields(std::make_index_sequence<field_count> { });
      }
    }

    std::vector<component_holder_t> update(StyleArchive& style_archive, component_builder_t& content_children_builder) override {
      ZoneScopedN("Update - Component EV");
      auto eh = static_cast<EventHandler*>(event_handler_.get());
      auto [at, ir] = component_data();
      return eh->on_redraw(
        at.x,
        at.y,
        ir.cw,
        ir.ch,
        at.padding_top,
        at.padding_bottom,
        at.padding_left,
        at.padding_right,
        content_children_builder
      );
    }
    void paint_fragment(vg::fragment_t &fragment) override {
      ZoneScopedN("Paint Fragment - Component EV");
      auto eh = static_cast<EventHandler*>(event_handler_.get());
      auto [at, ir] = component_data();
      eh->draw_fragment(
        fragment,
        dimensions::get_value(ir.cx),
        dimensions::get_value(ir.cy),
        dimensions::get_value(ir.cw),
        dimensions::get_value(ir.ch),
        dimensions::get_value(at.padding_top),
        dimensions::get_value(at.padding_bottom),
        dimensions::get_value(at.padding_left),
        dimensions::get_value(at.padding_right)
      );
    }

    void dispatch_key_press(const KeyEvent &ev) override {
      ZoneScopedN("Key Press - Component EV");
      auto eh = static_cast<EventHandler*>(event_handler_.get());
      auto [at, ir] = component_data();
      eh->on_key_press(
        ev,
        dimensions::get_value(at.x),
        dimensions::get_value(at.y),
        dimensions::get_value(ir.cw),
        dimensions::get_value(ir.ch),
        dimensions::get_value(at.padding_top),
        dimensions::get_value(at.padding_bottom),
        dimensions::get_value(at.padding_left),
        dimensions::get_value(at.padding_right)
      );
    }

    void dispatch_key_release(const KeyEvent &ev) override {
      ZoneScopedN("Key Release - Component EV");
      auto eh = static_cast<EventHandler*>(event_handler_.get());
      auto [at, ir] = component_data();
      eh->on_key_release(
        ev,
        dimensions::get_value(at.x),
        dimensions::get_value(at.y),
        dimensions::get_value(ir.cw),
        dimensions::get_value(ir.ch),
        dimensions::get_value(at.padding_top),
        dimensions::get_value(at.padding_bottom),
        dimensions::get_value(at.padding_left),
        dimensions::get_value(at.padding_right)
      );
    }

    void dispatch_text_input(const TextInputEvent &ev) override {
      ZoneScopedN("Text Input - Component EV");
      auto eh = static_cast<EventHandler*>(event_handler_.get());
      auto [at, ir] = component_data();
      eh->on_text_input(
        ev,
        dimensions::get_value(at.x),
        dimensions::get_value(at.y),
        dimensions::get_value(ir.cw),
        dimensions::get_value(ir.ch),
        dimensions::get_value(at.padding_top),
        dimensions::get_value(at.padding_bottom),
        dimensions::get_value(at.padding_left),
        dimensions::get_value(at.padding_right)
      );
    }

    void dispatch_button_press(
      const Button &button,
      dimension_t::value_type x,
      dimension_t::value_type y
    ) override {
      ZoneScopedN("Button Press - Component EV");
      auto eh = static_cast<EventHandler*>(event_handler_.get());
      auto [at, ir] = component_data();
      eh->on_button_press(
        button,
        x,
        y,
        dimensions::get_value(at.x),
        dimensions::get_value(at.y),
        dimensions::get_value(ir.cw),
        dimensions::get_value(ir.ch),
        dimensions::get_value(at.padding_top),
        dimensions::get_value(at.padding_bottom),
        dimensions::get_value(at.padding_left),
        dimensions::get_value(at.padding_right)
      );
    }

    void dispatch_button_release(
      const Button &button,
      dimension_t::value_type x,
      dimension_t::value_type y
    ) override {
      ZoneScopedN("Button Release - Component EV");
      auto eh = static_cast<EventHandler*>(event_handler_.get());
      auto [at, ir] = component_data();
      eh->on_button_release(
        button,
        x,
        y,
        dimensions::get_value(at.x),
        dimensions::get_value(at.y),
        dimensions::get_value(ir.cw),
        dimensions::get_value(ir.ch),
        dimensions::get_value(at.padding_top),
        dimensions::get_value(at.padding_bottom),
        dimensions::get_value(at.padding_left),
        dimensions::get_value(at.padding_right)
      );
    }

    void dispatch_mouse_enter(dimension_t::value_type x, dimension_t::value_type y) override {
      ZoneScopedN("Mouse Enter - Component EV");
      auto eh = static_cast<EventHandler*>(event_handler_.get());
      auto [at, ir] = component_data();
      eh->on_mouse_enter(
        x,
        y,
        dimensions::get_value(at.x),
        dimensions::get_value(at.y),
        dimensions::get_value(ir.cw),
        dimensions::get_value(ir.ch),
        dimensions::get_value(at.padding_top),
        dimensions::get_value(at.padding_bottom),
        dimensions::get_value(at.padding_left),
        dimensions::get_value(at.padding_right)
      );
    }

    void dispatch_mouse_exit(dimension_t::value_type x, dimension_t::value_type y) override {
      ZoneScopedN("Mouse Exit - Component EV");
      auto eh = static_cast<EventHandler*>(event_handler_.get());
      auto [at, ir] = component_data();
      eh->on_mouse_exit(
        x,
        y,
        dimensions::get_value(at.x),
        dimensions::get_value(at.y),
        dimensions::get_value(ir.cw),
        dimensions::get_value(ir.ch),
        dimensions::get_value(at.padding_top),
        dimensions::get_value(at.padding_bottom),
        dimensions::get_value(at.padding_left),
        dimensions::get_value(at.padding_right)
      );
    }

    void dispatch_mouse_motion(dimension_t::value_type x, dimension_t::value_type y) override {
      ZoneScopedN("Mouse Motion - Component EV");
      auto eh = static_cast<EventHandler*>(event_handler_.get());
      auto [at, ir] = component_data();
      eh->on_mouse_motion(
        x,
        y,
        dimensions::get_value(at.x),
        dimensions::get_value(at.y),
        dimensions::get_value(ir.cw),
        dimensions::get_value(ir.ch),
        dimensions::get_value(at.padding_top),
        dimensions::get_value(at.padding_bottom),
        dimensions::get_value(at.padding_left),
        dimensions::get_value(at.padding_right)
      );
    }

    void dispatch_scroll(dimension_t::value_type dx, dimension_t::value_type dy) override {
      ZoneScopedN("Scroll - Component EV");
      auto eh = static_cast<EventHandler*>(event_handler_.get());
      auto [at, ir] = component_data();
      eh->on_scroll(
        dx,
        dy,
        dimensions::get_value(at.x),
        dimensions::get_value(at.y),
        dimensions::get_value(ir.cw),
        dimensions::get_value(ir.ch),
        dimensions::get_value(at.padding_top),
        dimensions::get_value(at.padding_bottom),
        dimensions::get_value(at.padding_left),
        dimensions::get_value(at.padding_right)
      );
    }

    void dispatch_focus_changed() override {
      ZoneScopedN("Scroll - Component EV");
      auto eh = static_cast<EventHandler*>(event_handler_.get());
      auto [at, ir] = component_data();
      eh->on_focus_changed(
        dimensions::get_value(at.x),
        dimensions::get_value(at.y),
        dimensions::get_value(ir.cw),
        dimensions::get_value(ir.ch),
        dimensions::get_value(at.padding_top),
        dimensions::get_value(at.padding_bottom),
        dimensions::get_value(at.padding_left),
        dimensions::get_value(at.padding_right)
      );
    }

  private:
    std::pair<component_dimensional_relations_t, internal_relations_t&> component_data() {
      return {component_->get_dimensional_relations(), component_->get_internal_relations()};
    }

    template<std::size_t... I>
    void configure_event_handler_fields(std::index_sequence<I...>) {
      ZoneScopedN("Configure Fields");
      (configure_event_handler_field<I>(), ...);
    }

    template<std::size_t FieldI>
    void configure_event_handler_field() {
      using field         = refl::field<EventHandler, FieldI>;
      using field_type    = typename field::type;
      ZoneScopedN(field::name);
      auto* eh = static_cast<EventHandler*>(event_handler_.get());

      if constexpr (packtl::is_type<use_context, field_type>::value) {
        using context_type = typename field_type::context_type;
        using ret_context_type = typename field_type::ret_context_type;

        use_context<ret_context_type> &ctx_ref = field::from_instance(*eh);

        auto ctx = component_->find_context<context_type>();

        if (ctx.has_value()) {
          use_context_delegate::set_context(&ctx_ref, ctx.value());
          use_context_delegate::set_owns_context(&ctx_ref, false);
        } else {
          use_context_delegate::set_context(&ctx_ref, new context_type{});
          use_context_delegate::set_owns_context(&ctx_ref, true);
        }

        use_context_delegate::set_state(&ctx_ref, component_->state());
        use_context_delegate::start_listening(&ctx_ref);
      } else if constexpr (packtl::is_type<provide_context, field_type>::value) {
        using context_type = typename field_type::context_type;

        provide_context<context_type> &ctx_ref = field::from_instance(*eh);
        provide_context_delegate::set_state(&ctx_ref, component_->state());
      }
    }

  private:
    component_base_t* component_{};
    std::shared_ptr<event_handler_t> event_handler_{};
  };
}