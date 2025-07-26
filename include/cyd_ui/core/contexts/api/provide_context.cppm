// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

export module cydui.core.contexts.api:provide_context;

import std;

import fabric.logging;
import fabric.async;

export import cydui.core.contexts.events;
export import cydui.core.mounted;

namespace cydui::core {
  export struct provide_context_delegate;

  export template <typename ContextType>
  struct provide_context;

  template <typename ContextType>
  struct provide_context {
    using context_type = ContextType;

  private:
    // This object will own the context if it couldn't be found and thus a default one was created
    backends::frame_base::sptr    window_ = nullptr;
    std::shared_ptr<context_type> context_;

  public:
    friend struct core::provide_context_delegate;

    provide_context()
        : context_(std::make_shared<context_type>()) {}

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
      window_->emit<ContextUpdate<context_type>>({context_.get()});
    }

    operator std::shared_ptr<context_type>() {
      return context_;
    }
  };


  struct provide_context_delegate {
    template <typename ContextType>
    static void set_window(
      provide_context<ContextType>*     it,
      const backends::frame_base::sptr& window
    ) {
      it->window_ = window;
    }
    template <typename ContextType>
    static void set_context(
      provide_context<ContextType>*       it,
      const std::shared_ptr<ContextType>& ctx
    ) {
      it->context_ = ctx;
    }
  };
} // namespace cydui::core
