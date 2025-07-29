// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

export module cydui.core.blueprint.concepts;

import std;

export import cydui.styling.style_base;
export import cydui.core.blueprint.base;
export import cydui.core.state;

export namespace cydui::detail {
  template <typename T>
  concept StaticBlueprint =
    std::is_base_of_v<Blueprint, T> and requires { typename T::event_handler_t; };


  template <StaticBlueprint B>
  struct event_handler_type_t {
    using type = typename B::event_handler_t;
  };

  template <typename T>
  using event_handler_type = typename event_handler_type_t<T>::type;


  template <typename T>
  concept HasCustomStateType = StaticBlueprint<T> and requires {
    typename event_handler_type<T>::state_type;
  } and std::is_base_of_v<ComponentState, typename event_handler_type<T>::state_type>;

  template <typename T>
  concept HasCustomStyleType = StaticBlueprint<T> and requires {
    typename event_handler_type<T>::style_type;
  } and std::is_base_of_v<style::style_base_t, typename event_handler_type<T>::style_type>;


  template <typename B>
  struct state_type_t;

  template <typename B>
    requires HasCustomStateType<B>
  struct state_type_t<B> {
    using type = typename event_handler_type<B>::state_type;
  };

  template <typename B>
    requires(not HasCustomStateType<B>)
  struct state_type_t<B> {
    using type = ComponentState;
  };

  template <typename T>
  using state_type = typename state_type_t<T>::type;


  template <typename B>
  struct style_type_t;

  template <typename B>
    requires HasCustomStyleType<B>
  struct style_type_t<B> {
    using type = typename event_handler_type<B>::style_type;
  };

  template <typename B>
    requires(not HasCustomStyleType<B>)
  struct style_type_t<B> {
    using type = style::simple_style_t;
  };

  template <typename T>
  using style_type = typename style_type_t<T>::type;
} // namespace cydui::components

export template <typename, typename = void>
constexpr bool is_type_complete_v = false;

export template <typename T>
constexpr bool is_type_complete_v<T, std::void_t<decltype(sizeof(T))>> = true;
