// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include <tracy/Tracy.hpp>

export module cydui.components;

import std;

import fabric.logging;

export import :with_specialization;
export import cydui.components.base;
export import cydui.components.event_dispatcher;

namespace cyd::ui::components {
  export template<typename T>
  class component_t:
    public component_base_t,
    public attrs_component<T> {
  public:
    component_t() {
      internal_relations.cx = this->_x + this->_margin_left + this->_padding_left;
      internal_relations.cy = this->_y + this->_margin_top + this->_padding_top;

      style_data = std::make_shared<style_data_t<typename T::style_t>>();
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

    component_base_t*
    find_by_coords(dimension_t::value_type x, dimension_t::value_type y) final {
      using namespace dimensions;

      component_base_t* found = nullptr;
      for (auto c = children.rbegin(); c != children.rend(); ++c) {
        auto cx = get_value(c->get()->attrs()->_x);
        auto cy = get_value(c->get()->attrs()->_y);
        auto mx = get_value(c->get()->attrs()->_margin_left);
        auto my = get_value(c->get()->attrs()->_margin_top);
        auto px = get_value(c->get()->attrs()->_padding_left);
        auto py = get_value(c->get()->attrs()->_padding_top);
        found   = (*c)->find_by_coords(x - cx - mx - px, y - cy - my - py);
        if (nullptr != found) {
          return found;
        }
      }

      if (x < 0 ||
          x >= get_value(this->_width) ||
          y < 0 ||
          y >= get_value(this->_height)) {
        return nullptr;
      }
      return this;
    }

    attrs_dimensions<> &get_dimensional_relations() final {
      return *reinterpret_cast<attrs_dimensions<>*>(static_cast<attrs_dimensions<T>*>(this));
    }

    std::shared_ptr<dimension_ctx_t> get_dimensional_context() final {
      return this->dimension_ctx;
    }

    const refl::type_info& get_style_type_info() const final {
      using style_t = typename T::style_t;
      const auto& ti = refl::type_info::from<style_t>();
      return ti;
    }

  private:
    void mount() final {
      event_dispatcher.emplace(std::make_shared<event_dispatcher_t<T, typename T::event_handler_t>>(this));
    }
    void dismount() final {
      // Delete event handler, this component will now stop reacting to events
      event_dispatcher = std::nullopt;

      state()->component_instance = std::nullopt;
      get_dimensional_context()->clear_parameters();
    }

    bool update_with(std::shared_ptr<component_base_t> other) final {
      ZoneScopedN("Update With");
      auto other_component = std::dynamic_pointer_cast<component_t>(other);
      if (!other_component) {
        LOG::print {
          ERROR
        }("Attempted to update component of type ({}) with type ({})", this->name(), other->name());
        return false;
      }

      bool dirty = false;
      if (not refl::deep_eq(props(), other_component->props())) {
        props() = other_component->props();
        dirty = true;
      }
      if (not(*as_attrs() == *(other_component->as_attrs()))) {
        as_attrs()->update_with(*(other_component->as_attrs()));
        dirty = true;
      }

      if (not other_component->style_data->has_override()) {
        style_data->clear_style_override();
        dirty = true;
      } else if (not style_data->has_override()) {
        style_data->set_style_override_ptr(std::move(other_component->style_data->get_style_override_ptr()));
        dirty = true;
      } else {
        auto & self_so = *static_cast<typename T::style_t*>(style_data->get_style_override_ptr().get());
        auto & other_so = *static_cast<typename T::style_t*>(other_component->style_data->get_style_override_ptr().get());
        if (not refl::deep_eq(self_so, other_so)
            or not refl::deep_eq(
              style_data->get_style_override_as_base(),
              other_component->style_data->get_style_override_as_base()
            )) {
          style_data->set_style_override_ptr(std::move(other_component->style_data->get_style_override_ptr()));
          dirty = true;
        }
      }

      if (update_fields(other_component)) {
        dirty = true;
      }

      return dirty;
    }

    std::shared_ptr<component_state_t> create_state_instance() final {
      std::shared_ptr<component_state_t> state;
      if constexpr (requires { new typename T::state_t {std::declval<typename T::props_t*>()}; }) {
        state = std::shared_ptr<component_state_t> {
          new typename T::state_t(static_cast<typename T::props_t*>(get_props()))
        };
      } else {
        state = std::shared_ptr<component_state_t> {new typename T::state_t()};
      }
      state->set_component_name(this->name());
      set_state(state);
      return state;
    }

  public:
    void set_style_override(const auto& new_style) {
      auto &self_so = *dynamic_cast<style_data_t<typename T::style_t>*>(style_data.get());
      self_so.set_style_override(new_style);
    }

    T& operator[](const std::string& tag) {
      style_data->tags.insert(tag);
      return *dynamic_cast<T*>(this);
    }
    T& operator[](const std::unordered_set<std::string>& tags) {
      for (const auto & tag : tags) {
        style_data->tags.insert(tag);
      }
      return *dynamic_cast<T*>(this);
    }

    T& id(const std::string& id_) {
      this->set_id(id_);
      return *dynamic_cast<T*>(this);
    }
  protected:
    template <typename Fun>
    void set_style_transform(Fun&& transform_func) {
      auto &self_so = *dynamic_cast<style_data_t<typename T::style_t>*>(style_data.get());
      self_so.set_style_transform(std::forward<Fun>(transform_func));
    }
    void clear_style_transform() {
      auto &self_so = *dynamic_cast<style_data_t<typename T::style_t>*>(style_data.get());
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

    auto &props() {
      return (dynamic_cast<T*>(this)->props);
    }

    bool update_fields(std::shared_ptr<component_t> &other) {
      bool dirty = false;
      [&]<std::size_t ... I>(std::index_sequence<I...>) {
        (update_field<I>(dirty, other), ...);
      }(std::make_index_sequence<refl::field_count<T>>());
      return dirty;
    }

    template<std::size_t I>
    void update_field(bool &dirty, std::shared_ptr<component_t> &other_) {
      auto other       = std::dynamic_pointer_cast<T>(other_);
      using field      = refl::field<T, I>;
      using field_type = typename field::type;

      if constexpr (packtl::is_type<fabric::wiring::signal, field_type>::value) {
        auto &this_signal  = field::from_instance(*dynamic_cast<T*>(this));
        auto &other_signal = field::from_instance(*other);

        if (this_signal != other_signal) {
          this_signal = other_signal;
          dirty       = true;
        }
      }
    }
  };
}

export template<typename, typename = void>
constexpr bool is_type_complete_v = false;

export template<typename T>
constexpr bool is_type_complete_v
  <T, std::void_t<decltype(sizeof(T))>> = true;
