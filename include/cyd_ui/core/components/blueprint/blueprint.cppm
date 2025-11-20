// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include <tracy/Tracy.hpp>

#define STYLE_SETTER_RETURN_TYPE T&
#define STYLE_SETTER_RETURN_EXPR return as_derived();
#define STYLE_SETTER_REF_CONSTRAINT
#define STYLE_MAP_GETTER this->style_map_
#include "../../style/include/style_setters_detail.h"

export module cydui.core.blueprint;

import std;
export import reflect;

import fabric.logging;

export import cydui.core.blueprint.base;
export import cydui.core.event_dispatcher;
export import cydui.core.blueprint.concepts;

export import cydui.core.aspects.reference;

namespace cydui::detail {
  export template <typename T>
  class BlueprintImpl: public Blueprint {
  public:
    explicit BlueprintImpl(ComponentIdentifier identifier = {})
        : Blueprint(
            identifier,
            std::string {refl::type_name<T>},
            refl::type_info::from<style_type<T>>()) {}

    BlueprintImpl(const BlueprintImpl& other)
        : Blueprint(other) {}

  public:
    const refl::type_info& get_style_type_info() const final {
      const auto& ti = refl::type_info::from<style_type<T>>();
      return ti;
    }

    std::unique_ptr<event_dispatcher_base_t> make_event_dispatcher(
      fabric::async::async_bus_t& bus,
      event_dispatcher_base_t*    parent,
      void*                       component,
      ComponentState&             state,
      context_store_t&            context_store) const final {
      return std::make_unique<event_dispatcher_t<T, event_handler_type<T>>>(
        bus, parent, static_cast<Component*>(component), state, context_store);
    }

    ComponentState::sptr make_state_object() const final {
      std::shared_ptr<ComponentState> state;
      if constexpr (requires { new state_type<T> {props()}; }) {
        state = std::shared_ptr<ComponentState> {new state_type<T>(props())};
      } else {
        state = std::shared_ptr<ComponentState> {new state_type<T>()};
      }
      component_state_delegate_t::set_name(state.get(), get_name());

      return state;
    }

    style::style_object_t make_style_object() const final {
      return style::style_object_t {std::make_shared<style_type<T>>()};
    }

    update_result update_with(const Blueprint& other) final {
      ZoneScopedN("Update With");
      const auto* other_component = dynamic_cast<const BlueprintImpl<T>*>(&other);
      if (!other_component) {
        LOG::print {FATAL} //
        ("Attempted to update component of type ({}) with type ({})", this->get_name(), other.get_name());
        return {false, false};
      }

      bool dirty = false;
      bool restyle = false;
      if (not refl::deep_eq(props(), other_component->props())) {
        props() = other_component->props();
        dirty   = true;
      }

      if (not content_.empty() or not other_component->content_.empty()) {
        // TODO - Should diff content elements too?
        content_ = other_component->content_;
        dirty    = true;
      }

      if (style_map_ != other_component->style_map_) {
        style_map_ = other_component->style_map_;
        dirty      = true;
      }

      if (tags_ != other_component->tags_) {
        tags_ = other_component->tags_;
        restyle = true;
      }

      if (update_fields(other_component)) {
        dirty = true;
      }

      references_ = other_component->references_;

      return {dirty, restyle};
    }

    bool handles_text_input() const final {
      using EVH = typename T::event_handler_t;
      return EVH::handles_text_input;
    }

    uptr clone() const override {
      return std::make_unique<T>(as_derived());
    }

  private:
    auto& props() {
      return (as_derived().props);
    }

    const auto& props() const {
      return (as_derived().props);
    }

    bool update_fields(const BlueprintImpl<T>* other) {
      bool dirty = false;
      [&]<std::size_t... I>(std::index_sequence<I...>) {
        (update_field<I>(dirty, other), ...);
      }(std::make_index_sequence<refl::field_count<T>>());
      return dirty;
    }

    template <std::size_t I>
    void update_field(
      bool&                   dirty,
      const BlueprintImpl<T>* other_) {
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
      return refl::any_ref {as_derived().props};
    }

  public:
    T& tag(const std::unordered_set<std::string>& tags) {
      Blueprint::tag(tags);
      return as_derived();
    }

    T& tag(const std::string& tag) {
      Blueprint::tag(tag);
      return as_derived();
    }

    T& tag(
      const detail::tag_type& tag,
      bool                    tagged) {
      if (tagged) {
        this->tag(tag);
      } else {
        untag(tag);
      }
      return as_derived();
    }

    T& untag(const std::unordered_set<std::string>& tags) {
      Blueprint::untag(tags);
      return as_derived();
    }

    T& untag(const std::string& tag) {
      Blueprint::untag(tag);
      return as_derived();
    }

    T& set_id(const std::string& id) {
      Blueprint::set_id(id);
      return as_derived();
    }

    T& operator()(BlueprintList&& _content_) {
      this->content_ = _content_;
      return as_derived();
    }

    T& operator()(const BlueprintList& _content_) {
      this->content_ = _content_;
      return as_derived();
    }

    template <
      template <typename> typename R,
      std::same_as<T> S = T>
      requires std::derived_from<
        R<S>,
        reference<S>>
    S& ref(R<S>& reference_) {
      references_.insert(&reference_);
      return as_derived();
    }

#include "../../style/include/style_setters.inc"
  private:
    T& as_derived() {
      return *dynamic_cast<T*>(this);
    }

    const T& as_derived() const {
      return *dynamic_cast<const T*>(this);
    }
  };
} // namespace cydui::detail
