// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include <tracy/Tracy.hpp>

#define STYLE_SETTER_RETURN_TYPE T&
#define STYLE_SETTER_RETURN_EXPR return *static_cast<T*>(this);
#include "./style_setters_detail.h"

export module cydui.components;

import std;

import fabric.logging;

export import cydui.components.base;
export import cydui.components.anchors;
export import cydui.components.event_dispatcher;

namespace cydui::components {
  export template <typename T>
  class component_t: public component_base_t, public attrs_component<T> {
  public:
    explicit component_t(identifier_t identifier = {})
        : component_base_t(identifier) {
      style_data = std::make_shared<style_data_t<typename T::style_t>>(std::string{name()});

      auto dim_ctx = style_data->get_dimension_ctx();
      internal_relations.cx.set_context(dim_ctx, "cx");
      internal_relations.cy.set_context(dim_ctx, "cy");
      internal_relations.cw.set_context(dim_ctx, "cw");
      internal_relations.ch.set_context(dim_ctx, "ch");

      auto dim              = get_dimensional_relations();
      internal_relations.cx = dim.x + dim.margin_left + dim.padding_left;
      internal_relations.cy = dim.y + dim.margin_top + dim.padding_top;
    }

    ~component_t() override {
      children.clear();
      if (state()) {
        state()->component_instance = std::nullopt;
      }
    };

  public:
    attrs_component<>* attrs() final {
      // Yes, the order of casting matters here because a conversion from `this` to
      // `(attrs_component<>*)` does not work since that type is not a base of this
      // class. So we need to cast to the base class first and then to its `void`
      // specialization.
      return reinterpret_cast<attrs_component<>*>(as_attrs());
    }

    std::string name() const final {
      return std::string{refl::type_name<T>};
    }

    component_base_t* find_by_coords(dimension_t::value_type x, dimension_t::value_type y) final {
      using namespace dimensions;

      auto& style = get_style();

      if (style.overflow_x != overflow_e::GROW) {
        if (x < 0 || x >= get_value(style.width)) {
          return nullptr;
        }
      }
      if (style.overflow_y != overflow_e::GROW) {
        if (y < 0 || y >= get_value(style.height)) {
          return nullptr;
        }
      }

      auto              sx    = get_value(style.scroll_x);
      auto              sy    = get_value(style.scroll_y);
      component_base_t* found = nullptr;
      for (auto c = children.rbegin(); c != children.rend(); ++c) {
        auto dim = c->get()->get_dimensional_relations();
        auto cx  = get_value(dim.x);
        auto cy  = get_value(dim.y);
        auto mx  = get_value(dim.margin_left);
        auto my  = get_value(dim.margin_top);
        auto px  = get_value(dim.padding_left);
        auto py  = get_value(dim.padding_top);
        found    = (*c)->find_by_coords(x - cx - mx - px + sx, y - cy - my - py + sy);
        if (nullptr != found) {
          return found;
        }
      }

      if (x < 0 || x >= get_value(style.width) || y < 0 || y >= get_value(style.height)) {
        return nullptr;
      }
      return this;
    }

    component_dimensional_relations_t get_dimensional_relations() final {
      style_base_t& s = style_data->as_base();
      return {
        .x              = s.x,
        .y              = s.y,
        .width          = s.width,
        .height         = s.height,
        // .fixed_w = ,
        // .fixed_h = ,
        .margin_top     = s.margin.top,
        .margin_bottom  = s.margin.bottom,
        .margin_left    = s.margin.left,
        .margin_right   = s.margin.right,
        .padding_top    = s.padding.top,
        .padding_bottom = s.padding.bottom,
        .padding_left   = s.padding.left,
        .padding_right  = s.padding.right,
        .scroll_x       = s.scroll_x,
        .scroll_y       = s.scroll_y,
      };
    }

    std::shared_ptr<dimension_ctx_t> get_dimensional_context() final {
      return style_data->get_dimensional_ctx();
    }

    const refl::type_info& get_style_type_info() const final {
      using style_t  = typename T::style_t;
      const auto& ti = refl::type_info::from<style_t>();
      return ti;
    }

  private:
    void mount() final {
      event_dispatcher.emplace(
        std::make_shared<event_dispatcher_t<T, typename T::event_handler_t>>(this)
      );
      component_builder_t content_children_builder{};
      {
        std::vector<component_builder_t>& content_children = this->_content;
        for (auto& item: content_children) {
          for (auto& component: item.get_component_constructors()) {
            content_children_builder.append_component(component);
          }
        }
      }
      event_dispatcher.value()->dispatch_mount(content_children_builder);
    }
    void dismount() final {
      event_dispatcher.value()->dispatch_dismount();

      for (const auto& c: children) {
        component_actor_t::dismount_component(c.get());
      }
      children.clear();

      // Delete event handler, this component will now stop reacting to events
      event_dispatcher = std::nullopt;

      state()->component_instance = std::nullopt;
    }

    bool update_with(std::shared_ptr<component_base_t> other) final {
      ZoneScopedN("Update With");
      auto other_component = std::dynamic_pointer_cast<component_t>(other);
      if (!other_component) {
        LOG::print{ERROR
        }("Attempted to update component of type ({}) with type ({})", this->name(), other->name());
        return false;
      }

      bool dirty = false;
      if (not refl::deep_eq(props(), other_component->props())) {
        props() = other_component->props();
        dirty   = true;
      }
      if (not(*as_attrs() == *(other_component->as_attrs()))) {
        as_attrs()->update_with(*(other_component->as_attrs()));
        dirty = true;
      }

      if (style_data->update_override_with(other_component->style_data->style_override_data)) {
        dirty = true;
      }

      if (update_fields(other_component)) {
        dirty = true;
      }

      return dirty;
    }

    std::shared_ptr<component_state_t> create_state_instance() final {
      std::shared_ptr<component_state_t> state;
      if constexpr (requires { new typename T::state_t{std::declval<typename T::props_t*>()}; }) {
        state = std::shared_ptr<component_state_t>{
          new typename T::state_t(static_cast<typename T::props_t*>(get_props()))
        };
      } else {
        state = std::shared_ptr<component_state_t>{new typename T::state_t()};
      }
      state->set_component_name(this->name());

      using EVH = typename T::event_handler_t;
      if (EVH::handles_text_input) {
        // LOG::print {DEBUG}("Component {} handles text input", name());
        component_state_delegate_t::set_is_text_input(state.get(), true);
      }

      set_state(state);
      return state;
    }

  public:
    T& tag(const std::unordered_set<std::string>& tags) {
      for (const auto& tag: tags) {
        style_data->tags.insert(tag);
      }
      return *dynamic_cast<T*>(this);
    }

    T& tag(const std::string& tag_) {
      style_data->tags.insert(tag_);
      return *dynamic_cast<T*>(this);
    }

    T& untag(const std::unordered_set<std::string>& tags) {
      for (const auto& tag: tags) {
        style_data->tags.erase(tag);
      }
      return *dynamic_cast<T*>(this);
    }

    T& untag(const std::string& tag_) {
      style_data->tags.erase(tag_);
      return *dynamic_cast<T*>(this);
    }

    T& set_id(const std::string& id_) {
      component_base_t::set_id(id_);
      return *dynamic_cast<T*>(this);
    }

  protected:
    template <typename Fun>
    void set_style_transform(Fun&& transform_func) {
      auto& self_so = *dynamic_cast<style_data_t<typename T::style_t>*>(style_data.get());
      self_so.set_style_transform(std::forward<Fun>(transform_func));
    }
    void clear_style_transform() {
      auto& self_so = *dynamic_cast<style_data_t<typename T::style_t>*>(style_data.get());
      self_so.clear_style_transform();
    }

  private:
    attrs_component<T>* as_attrs() {
      // Yes, the order of casting matters here because a conversion from `this` to
      // `(attrs_component<>*)` does not work since that type is not a base of this
      // class. So we need to cast to the base class first and then to its `void`
      // specialization.
      return static_cast<attrs_component<T>*>(this);
    }

    auto& props() {
      return (dynamic_cast<T*>(this)->props);
    }

    bool update_fields(std::shared_ptr<component_t>& other) {
      bool dirty = false;
      [&]<std::size_t... I>(std::index_sequence<I...>) {
        (update_field<I>(dirty, other), ...);
      }(std::make_index_sequence<refl::field_count<T>>());
      return dirty;
    }

    template <std::size_t I>
    void update_field(bool& dirty, std::shared_ptr<component_t>& other_) {
      auto other       = std::dynamic_pointer_cast<T>(other_);
      using field      = refl::field<T, I>;
      using field_type = typename field::type;

      if constexpr (packtl::is_type<fabric::wiring::signal, field_type>::value) {
        auto& this_signal  = field::from_instance(*dynamic_cast<T*>(this));
        auto& other_signal = field::from_instance(*other);

        if (this_signal != other_signal) {
          this_signal = other_signal;
          dirty       = true;
        }
      }
    }

  public:
#include "./style_setters.inc"
  };
} // namespace cydui::components

export template <typename, typename = void>
constexpr bool is_type_complete_v = false;

export template <typename T>
constexpr bool is_type_complete_v<T, std::void_t<decltype(sizeof(T))>> = true;
