// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include <tracy/Tracy.hpp>

#define STYLE_SETTER_RETURN_TYPE T&
#define STYLE_SETTER_RETURN_EXPR return *static_cast<T*>(this);
#define STYLE_SETTER_REF_CONSTRAINT
#define STYLE_MAP_GETTER this->style_map_
#include "../styling/style_setters_detail.h"

export module cydui.components.blueprint;

import std;
export import reflect;

import fabric.logging;

export import cydui.components.blueprint.event_dispatcher;
export import cydui.components.blueprint.concepts;

namespace cydui::components {
  export template <typename T>
  class blueprint_t: public blueprint_base_t {
  public:
    explicit blueprint_t(identifier_t identifier = {})
        : blueprint_base_t(
            identifier,
            std::string{refl::type_name<T>},
            refl::type_info::from<style_type<T>>()
          ) {}

    blueprint_t(const blueprint_t& other)
        : blueprint_base_t(other) {}

  public:
    const refl::type_info& get_style_type_info() const final {
      const auto& ti = refl::type_info::from<style_type<T>>();
      return ti;
    }

    std::unique_ptr<event_dispatcher_base_t> make_event_dispatcher(void* component) const final {
      return std::make_unique<event_dispatcher_t<T, event_handler_type<T>>>(
        static_cast<mounted_component_t*>(component)
      );
    }

    component_state_t::sptr make_state_object() const final {
      std::shared_ptr<component_state_t> state;
      if constexpr (requires { new state_type<T>{props()}; }) {
        state = std::shared_ptr<component_state_t>{new state_type<T>(props())};
      } else {
        state = std::shared_ptr<component_state_t>{new state_type<T>()};
      }
      component_state_delegate_t::set_name(state.get(), get_name());

      return state;
    }

    style_object_t make_style_object() const final {
      return style_object_t{std::make_shared<style_type<T>>()};
    }

    bool update_with(const blueprint_base_t& other) final {
      ZoneScopedN("Update With");
      const auto* other_component = dynamic_cast<const blueprint_t<T>*>(&other);
      if (!other_component) {
        LOG::print{FATAL} //
        ("Attempted to update component of type ({}) with type ({})",
         this->get_name(),
         other.get_name());
        return false;
      }

      bool dirty = false;
      if (not refl::deep_eq(props(), other_component->props())) {
        props() = other_component->props();
        dirty   = true;
      }

      // TODO - Should diff content too

      if (style_map_ != other_component->style_map_) {
        style_map_ = other_component->style_map_;
        dirty      = true;
      }

      if (update_fields(other_component)) {
        dirty = true;
      }

      return dirty;
    }

    bool handles_text_input() const final {
      using EVH = typename T::event_handler_t;
      return EVH::handles_text_input;
    }

    uptr clone() const override {
      return std::make_unique<T>(*dynamic_cast<const T*>(this));
    }

  private:
    auto& props() {
      return (dynamic_cast<T*>(this)->props);
    }

    const auto& props() const {
      return (dynamic_cast<const T*>(this)->props);
    }

    bool update_fields(const blueprint_t<T>* other) {
      bool dirty = false;
      [&]<std::size_t... I>(std::index_sequence<I...>) {
        (update_field<I>(dirty, other), ...);
      }(std::make_index_sequence<refl::field_count<T>>());
      return dirty;
    }

    template <std::size_t I>
    void update_field(
      bool&                 dirty,
      const blueprint_t<T>* other_
    ) {
      const auto* other = dynamic_cast<const T*>(other_);
      using field       = refl::field<T, I>;
      using field_type  = typename field::type;

      if constexpr (packtl::is_type<fabric::wiring::signal, field_type>::value) {
        auto&       this_signal  = field::from_instance(*dynamic_cast<T*>(this));
        const auto& other_signal = field::from_instance(*other);

        if (this_signal != other_signal) {
          this_signal = other_signal;
          dirty       = true;
        }
      }
    }

  public:
    refl::any_ref get_props() final {
      return refl::any_ref{dynamic_cast<T*>(this)->props};
    }

  public:
    T& tag(const std::unordered_set<std::string>& tags) {
      blueprint_base_t::tag(tags);
      return *dynamic_cast<T*>(this);
    }

    T& tag(const std::string& tag) {
      blueprint_base_t::tag(tag);
      return *dynamic_cast<T*>(this);
    }

    T& untag(const std::unordered_set<std::string>& tags) {
      blueprint_base_t::untag(tags);
      return *dynamic_cast<T*>(this);
    }

    T& untag(const std::string& tag) {
      blueprint_base_t::untag(tag);
      return *dynamic_cast<T*>(this);
    }

    T& set_id(const std::string& id) {
      blueprint_base_t::set_id(id);
      return *dynamic_cast<T*>(this);
    }

    T& operator()(content_type&& _content_) {
      this->content_ = _content_;
      return *dynamic_cast<T*>(this);
    }

    T& operator()(content_type& _content_) {
      this->content_ = _content_;
      return *dynamic_cast<T*>(this);
    }

#include "../styling/style_setters.inc"
  };
} // namespace cydui::components
