// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include <tracy/Tracy.hpp>
#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

export module cydui.application;

export import std;

export import fabric.async;
export import fabric.logging;

export import cydui.window_events;

export namespace cydui {
  class Application: public fabric::async::async_bus_t {
  private:
    Application()
        : stop_application_listener_(on_event([&](const StopApplicationEvent&) -> fabric::task<> {
            if (this->window_map_.empty()) {
              this->emit<fabric::async::StopBusEvent>();
            }
            this->stop_application_flag_.test_and_set();
            this->stop_application_flag_.notify_all();
            co_return;
          })) {
      ZoneScopedN("Application{}");
      using namespace std::chrono_literals;
      LOG::print{INFO}("Initializing application...");

      get_executor()
        ->schedule([] -> fabric::task<> {
          tracy::SetThreadNameWithHint("Application", 1);
          ZoneScopedN("Application Init");
          SDL_SetMainReady();
          if (not SDL_Init(SDL_INIT_VIDEO)) {
            SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
            LOG::print{ERROR}("Couldn't initialize SDL3: {}", SDL_GetError());
          }
          LOG::print{INFO}("SDL3 initialized.");
          co_return;
        })
        .wait();

      get_executor()->schedule([&] -> fabric::task<> {
        while (true) { // will be stopped at the co_await when the executor is destroyed
          {
            std::scoped_lock lk{get_instance().window_map_mtx_};
            window_events::poll_events(this, &get_instance().window_map_);
          }
          co_await 16ms;
        }
        co_return;
      });
      LOG::print{INFO}("Application initialized...");
    }

  public:
    ~Application() {
      ZoneScopedN("~Application");
    }

  public:
    static Application& get_instance() {
      static Application instance;
      return instance;
    }

    template <typename... Args>
    static auto run_async(auto&& fun, Args&&... args) {
      ZoneScopedN("Application:run_async");
      return get_instance()->schedule(
        [=](Args... argss) -> fabric::task<decltype(fun(std::forward<Args>(argss)...))> {
          ZoneScopedN("Application:run_async:()");
          co_return fun(std::forward<Args>(argss)...);
        },
        std::forward<Args>(args)...
      );
    }

    template <typename... Args>
    static auto run(auto&& fun, Args&&... args) {
      ZoneScopedN("Application:run");
      return run_async(fun, std::forward<Args>(args)...).get();
    }

    template <typename... Args>
    static auto schedule(auto&& fun, Args&&... args) {
      ZoneScopedN("Application:run_async");
      return get_instance()->schedule(fun, std::forward<Args>(args)...);
    }

    template <typename... Args>
    static auto schedule(fabric::tasks::time_point tp, auto&& fun, Args&&... args) {
      ZoneScopedN("Application:run_async");
      return get_instance()->schedule(tp, fun, std::forward<Args>(args)...);
    }

    template <typename... Args>
    static auto schedule(fabric::tasks::duration duration, auto&& fun, Args&&... args) {
      ZoneScopedN("Application:run_async");
      return get_instance()->schedule(duration, fun, std::forward<Args>(args)...);
    }

    static void register_window(std::size_t id, fabric::async::async_bus_t* window) {
      ZoneScopedN("Application:register_window");
      std::scoped_lock lk{get_instance().window_map_mtx_};
      get_instance().window_map_[id] = window;
      LOG::print{INFO}("Window registered ID={}", id);
    }
    static void unregister_window(std::size_t id) {
      ZoneScopedN("Application:unregister_window");
      Application&     instance = get_instance();
      std::scoped_lock lk{instance.window_map_mtx_};
      instance.window_map_.erase(id);
      if (instance.window_map_.empty() and instance.stop_application_flag_.test()) {
        instance.stop_application_flag_.clear();
        instance.emit<fabric::async::StopBusEvent>();
      }
      LOG::print{INFO}("Window unregistered ID={}", id);
    }

    static void terminate() {
      get_instance()->request_stop();
    }

  private:
    TracyLockable(std::mutex, window_map_mtx_);
    std::map<std::size_t, fabric::async::async_bus_t*> window_map_{};
    fabric::async::listener<StopApplicationEvent>      stop_application_listener_;
    std::atomic_flag                                   stop_application_flag_{false};
  };
} // namespace cydui

// using cyd::fabric::async::async;
