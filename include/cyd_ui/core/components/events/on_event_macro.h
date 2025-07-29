// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CYD_UI_ON_EVENT_MACRO_H
#define CYD_UI_ON_EVENT_MACRO_H

#define ON_EVENT(EVENT, ...)                                                                       \
  cydui::detail::custom_event_listener<EVENT> on_##EVENT {                                         \
    this->bus,                                                                                     \
    [&](const EVENT& event) -> fabric::task<> {                                                    \
      __VA_ARGS__;                                                                                 \
      co_return;                                                                                   \
    },                                                                                             \
    [&]() -> fabric::task<> {                                                                      \
      this->component.mark_dirty();                                                                \
      co_return;                                                                                   \
    }};

#endif // CYD_UI_ON_EVENT_MACRO_H
