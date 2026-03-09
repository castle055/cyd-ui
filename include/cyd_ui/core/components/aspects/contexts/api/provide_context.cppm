// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

export module cydui.core.aspects.contexts:provide_context;

import std;
import reflect;

import fabric.logging;
import fabric.async;

import cydui.event_types;
export import cydui.core.aspects;
export import cydui.core.aspects.contexts.events;

namespace cydui {
  export template <typename ContextType>
  struct provide_context: ComponentAspect {
    static constexpr bool is_reference = std::is_lvalue_reference_v<ContextType>;
    using context_type                 = ContextType;
    using stored_type = std::conditional_t<is_reference, std::remove_reference_t<ContextType>*, ContextType>;

  private:
    std::shared_ptr<stored_type>                                       context_;
    std::optional<fabric::async::listener<ContextUpdate<stored_type>>> listener {std::nullopt};

    void on_mount() override {
      start_listening();
      get_context_store().template add_context<stored_type>(context_);
    }

  public:
    provide_context()
      requires(not is_reference)
        : context_(std::make_shared<stored_type>()) {}

    template <typename... Args>
      requires(
        not is_reference
        and std::constructible_from<
          stored_type,
          Args...>)
    explicit provide_context(Args&&... args)
        : context_(std::make_shared<stored_type>(std::forward<Args>(args)...)) {}

    explicit provide_context(context_type ref)
      requires is_reference
        : context_(std::make_shared<stored_type>(&ref)) {}

    ~provide_context() override {
      stop_listening();
    }

    // provide_context(std::function<void()>) {}

    // template <typename T, typename F>
    // provide_context(bind<T, F> b) {}

    provide_context(const provide_context& other) = default;

    provide_context& operator=(const provide_context& other) = default;

    provide_context(provide_context&& other) noexcept = default;

    provide_context& operator=(provide_context&& other) = default;

    std::conditional_t<
      is_reference,
      std::remove_reference_t<context_type>,
      context_type>*
    operator->() {
      if constexpr (is_reference) {
        return *context_;
      } else {
        return context_.get();
      }
    }

    std::conditional_t<
      is_reference,
      std::remove_reference_t<context_type>,
      context_type>&
    operator*() {
      if constexpr (is_reference) {
        return **context_;
      } else {
        return *context_;
      }
    }

    void notify() {
      get_bus().template emit<ContextUpdate<stored_type>>({context_.get()});
    }

    operator std::shared_ptr<stored_type>() {
      return context_;
    }

  private:
    void start_listening() {
      stop_listening();

      listener = get_bus().on_event([=, this](ContextUpdate<stored_type> ev) -> fabric::task<> {
        if (ev.ptr == context_.get()) {
          update_component();
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
