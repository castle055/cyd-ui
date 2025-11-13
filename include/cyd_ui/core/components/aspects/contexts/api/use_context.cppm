// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

export module cydui.core.aspects.contexts:use_context;

import std;
import reflect;

import fabric.logging;
import fabric.async;

import cydui.core.state;
export import cydui.core.aspects;
export import cydui.core.aspects.contexts.store;
export import cydui.core.aspects.contexts.events;


namespace cydui {
  export template <typename ContextType>
  struct use_context: private ComponentAspect {
    using ret_context_type            = ContextType;
    using context_type                = std::remove_const_t<ContextType>;
    static constexpr bool is_readonly = std::is_const_v<ContextType>;

  private:
    // This object will own the context if it couldn't be found and thus a default one was created
    bool          owns_context = false;
    context_type* ctx          = nullptr;

    std::optional<fabric::async::listener<ContextUpdate<context_type>>> listener {std::nullopt};

    void on_mount() override {
      auto ctx_opt = get_context_store().template find_context<context_type>();

      if (ctx_opt.has_value()) {
        ctx          = ctx_opt.value().get();
        owns_context = false;
      } else {
        ctx          = new context_type {};
        owns_context = true;
      }

      start_listening();
    }

  public:
    use_context() = default;

    use_context(const use_context& other) = delete;

    use_context& operator=(const use_context& other) = delete;

    use_context(use_context&& other) noexcept
        : ctx(other.ctx) {
      other.stop_listening();
      if (other.owns_context) {
        this->owns_context = true;

        other.owns_context = false;
        other.ctx          = nullptr;
      }
    }

    use_context& operator=(use_context&& other) noexcept {
      other.stop_listening();
      stop_listening();
      this->ctx = other.ctx;
      if (other.owns_context) {
        this->owns_context = true;
        other.owns_context = false;
      }
      return *this;
    }

    ~use_context() override {
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
      get_bus().template emit<ContextUpdate<context_type>>({ctx});
    }

    bool is_provided() const {
      return not owns_context;
    }

  private:
    void start_listening() {
      stop_listening();

      listener = get_bus().on_event([=, this](ContextUpdate<context_type> ev) -> fabric::task<> {
        if (ev.ptr == ctx) {
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
