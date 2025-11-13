// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

export module cydui.core.aspects.process;

import std;
import reflect;

import fabric.logging;
import fabric.async;
import fabric.exception;

import cydui.core.state;
export import cydui.core.aspects;
export import cydui.core.aspects.contexts.store;
export import cydui.core.aspects.contexts.events;


namespace cydui {
  export template <typename T, typename... Args>
  class process;

  export template <typename R, typename... Args>
  class process<R(Args...)>: private ComponentAspect {
  public:
    struct opts_t {
      std::function<fabric::task<R>(Args...)> fn;
      std::optional<std::tuple<Args...>>      auto_trigger {std::nullopt};
      R*                                      output {nullptr};
    };

  private:
    opts_t                 opts {};
    bool                   valid_ {false};
    fabric::shared_task<R> task_ {nullptr};

    void on_mount() override {
      if (opts.auto_trigger.has_value()) {
        std::apply([&](auto... args_) { trigger(args_...); }, opts.auto_trigger.value());
      }
    }

  public:
    process() = default;

    explicit process(const std::function<fabric::task<R>(Args...)>& fn)
        : opts(fn),
          valid_(true) {}

    explicit process(
      const std::function<fabric::task<R>(Args...)>& fn,
      std::tuple<Args...>                            args)
        : opts(
            fn,
            args),
          valid_(true) {}

    explicit process(const opts_t& opts)
        : opts(opts),
          valid_(true) {}

    void update_process(const std::function<fabric::task<R>(Args...)>& fn) {
      opts.fn = fn;
      valid_  = true;
    }

    bool running() const {
      return task_ != nullptr and not task_->done();
    }

    bool done() const {
      return task_ != nullptr and task_->done();
    }

    R& result() {
      return task_->get();
    }

    bool trigger(Args... args) {
      if (running() or not valid_) {
        return false;
      }
      task_ = get_bus()
                ->schedule([this](Args... args_) -> fabric::task<R> {
                  auto res = co_await opts.fn(args_...);
                  update_component();
                  if (nullptr != opts.output) {
                    if constexpr (requires(R r) { r.update_from_process(std::declval<decltype(res)>()); }) {
                      (*opts.output).update(res);
                    } else if constexpr (std::is_copy_assignable_v<R>) {
                      (*opts.output) = res;
                    } else if constexpr (std::is_move_assignable_v<R>) {
                      (*opts.output) = std::move(res);
                    } else {
                      throw fabric::exception(
                        std::format("Invalid output pointer: '{}' cannot be updated", refl::type_name<R>));
                    }
                  }
                  co_return res;
                }(args...))
                .share();
      return true;
    }
  };
} // namespace cydui
