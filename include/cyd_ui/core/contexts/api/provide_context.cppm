// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

export module cydui.core.contexts.api:provide_context;

import std;
import reflect;

import fabric.logging;
import fabric.async;

import cydui.event_types;
export import cydui.core.contexts.events;

namespace cydui::detail {
  export struct provide_context_delegate;
}

namespace cydui {
  export template <typename ContextType>
  struct provide_context {
    using context_type = ContextType;

  private:
    fabric::async::async_bus_t*                                         bus_ {nullptr};
    std::shared_ptr<context_type>                                       context_;
    std::optional<fabric::async::listener<ContextUpdate<context_type>>> listener {std::nullopt};

  public:
    friend struct detail::provide_context_delegate;

    provide_context()
        : context_(std::make_shared<context_type>()) {}

    template <typename... Args>
    explicit provide_context(Args&&... args)
        : context_(std::make_shared<context_type>(std::forward<Args>(args)...)) {}

    ~provide_context() {
      stop_listening();
    }

    // provide_context(std::function<void()>) {}

    // template <typename T, typename F>
    // provide_context(bind<T, F> b) {}

    provide_context(const provide_context& other) = default;

    provide_context& operator=(const provide_context& other) = default;

    provide_context(provide_context&& other) noexcept = default;

    provide_context& operator=(provide_context&& other) = default;

    context_type* operator->() {
      return context_.get();
    }

    context_type& operator*() {
      return *context_;
    }

    void notify() {
      bus_->emit<ContextUpdate<context_type>>({context_.get()});
      bus_->emit<RedrawEvent>({});
    }

    operator std::shared_ptr<context_type>() {
      return context_;
    }

  private:
    void start_listening(auto&& callback) {
      stop_listening();

      listener = bus_->on_event([=, this](ContextUpdate<context_type> ev) -> fabric::task<> {
        if (ev.ptr == context_.get()) {
          callback();
        }
        co_return;
      });
    }
    void stop_listening() {
      if (listener.has_value()) {
        listener.value().remove();
        listener = std::nullopt;
      }
    }
  };
} // namespace cydui

namespace cydui::detail {
  struct provide_context_delegate {
    template <typename ContextType>
    static void set_bus(
      provide_context<ContextType>* it,
      fabric::async::async_bus_t&   bus) {
      it->bus_ = &bus;
    }
    template <typename ContextType>
    static void set_context(
      provide_context<ContextType>*       it,
      const std::shared_ptr<ContextType>& ctx) {
      it->context_ = ctx;
    }
    template <typename ContextType>
    static void start_listening(
      provide_context<ContextType>* it,
      auto&&                        callback) {
      it->start_listening(callback);
    }
  };
} // namespace cydui::detail
