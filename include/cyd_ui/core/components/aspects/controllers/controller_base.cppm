// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

export module cydui.core.aspects.controller.base;

import std;
import reflect;

import cydui.core.blueprint.concepts;

namespace cydui {
  namespace detail {
    template <typename T>
    struct controller_state_t;

    template <typename B>
    struct controller_state_type_t;

    template <typename B>
      requires EventHandlerHasCustomStateType<B>
    struct controller_state_type_t<B> {
      using type = typename B::state_type;
    };

    template <typename B>
      requires(not EventHandlerHasCustomStateType<B>)
    struct controller_state_type_t<B> {
      using type = ComponentState;
    };

    template <typename T>
    using controller_state_type = typename controller_state_type_t<T>::type;
  } // namespace detail

  export template <typename T>
  class ControllerBase {
    T& evh_;

  public:
    explicit ControllerBase(T& event_handler)
        : evh_(event_handler),
          state(event_handler.state) {}

    void update_component() {
      evh_.component.mark_dirty();
      evh_.bus.emit(RedrawEvent {});
    }

  protected:
    detail::controller_state_type<T>& state;
  };
} // namespace cydui
