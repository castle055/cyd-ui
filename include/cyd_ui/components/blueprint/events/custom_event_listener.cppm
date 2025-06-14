// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

export module cydui.components.blueprint.custom_event_listener;

import std;
import reflect;

export import cydui.backends.frame_base;

export template <typename Event>
class custom_event_listener {
public:
  custom_event_listener(
    cydui::backends::frame_base&                window,
    std::function<fabric::task<>(const Event&)> callback,
    std::function<fabric::task<>()>             post
  )
      : window_(window),
        callback_(callback),
        post_(post) {
    start_listening();
  }

  ~custom_event_listener() {
    stop_listening();
  }

  custom_event_listener(const custom_event_listener& other)
      : window_(other.window_),
        callback_(other.callback_),
        post_(other.post_) {
    start_listening();
  }
  custom_event_listener& operator=(const custom_event_listener& other) {
    stop_listening();
    this->callback_ = other.callback_;
    this->post_     = other.post_;
    start_listening();
    return *this;
  }

  custom_event_listener(custom_event_listener&& other) noexcept
      : window_(other.window_),
        callback_(other.callback_),
        post_(other.post_) {
    other.stop_listening();
    start_listening();
  }
  custom_event_listener& operator=(custom_event_listener&& other) {
    stop_listening();
    this->callback_ = other.callback_;
    this->post_     = other.post_;
    other.stop_listening();
    start_listening();
    return *this;
  }

private:
  void start_listening() {
    stop_listening();

    listener_ = window_.on_event([&](const Event& ev) -> fabric::task<> {
      co_await callback_(ev);
      co_await post_();
      co_return;
    });
  }
  void stop_listening() {
    if (listener_.has_value()) {
      listener_.value().remove();
      listener_ = std::nullopt;
    }
  }

private:
  cydui::backends::frame_base&                  window_;
  std::function<fabric::task<>(const Event&)>   callback_;
  std::function<fabric::task<>()>               post_;
  std::optional<fabric::async::listener<Event>> listener_{std::nullopt};
};
