// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

export module cydui.components.contexts.api:use_context;

import std;

import fabric.logging;
import fabric.async;

export import cydui.components.contexts.events;
export import cydui.components.state;

namespace cydui {
  export struct use_context_delegate;

  export template <typename ContextType>
  struct use_context {
    using ret_context_type            = ContextType;
    using context_type                = std::remove_const_t<ContextType>;
    static constexpr bool is_readonly = std::is_const_v<ContextType>;
  private:
    // This object will own the context if it couldn't be found and thus a default one was created
    bool                       owns_context = false;
    context_type*              ctx          = nullptr;
    backends::frame_base::sptr window_      = nullptr;

    std::optional<fabric::async::listener<ContextUpdate<context_type>>> listener{std::nullopt};

  public:
    friend struct use_context_delegate;

    use_context() = default;

    use_context(const use_context& other) = delete;

    use_context& operator=(const use_context& other) = delete;

    use_context(use_context&& other) noexcept
        : ctx(other.ctx),
          window_(other.window_) {
      other.stop_listening();
      if (other.owns_context) {
        this->owns_context = true;

        other.owns_context = false;
        other.ctx          = nullptr;
      }
    }

    use_context& operator=(use_context&& other) {
      other.stop_listening();
      stop_listening();
      this->ctx   = other.ctx;
      this->window_ = other.window_;
      if (other.owns_context) {
        this->owns_context = true;
        other.owns_context = false;
      }
      return *this;
    }

    ~use_context() {
      stop_listening();
      if (owns_context) {
        delete ctx;
      }
    }

    ret_context_type* operator->() {
      return ctx;
    }

    ret_context_type& operator*() {
      return *ctx;
    }

    void notify() {
      window_->emit<ContextUpdate<context_type>>({ctx});
    }

  private:
    void start_listening(auto&& callback) {
      stop_listening();

      listener = window_->on_event([&](ContextUpdate<context_type> ev) -> fabric::task<> {
        if (ev.ptr == ctx) {
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


  struct use_context_delegate {
    template <typename ContextType>
    static void set_window(
      use_context<ContextType>*         it,
      const backends::frame_base::sptr& window
    ) {
      it->window_ = window;
    }
    template <typename ContextType>
    static void set_context(
      use_context<ContextType>* it,
      ContextType*              ctx
    ) {
      it->ctx = ctx;
    }
    template <typename ContextType>
    static void set_context(
      use_context<const ContextType>* it,
      ContextType*                    ctx
    ) {
      it->ctx = ctx;
    }
    template <typename ContextType>
    static void set_owns_context(
      use_context<ContextType>* it,
      bool                      value
    ) {
      it->owns_context = value;
    }
    template <typename ContextType>
    static void start_listening(use_context<ContextType>* it, auto&& callback) {
      it->start_listening(callback);
    }
  };
} // namespace cydui
