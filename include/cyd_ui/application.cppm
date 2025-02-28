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
    struct WindowEventSystem final: fabric::async::system_base_t {
      WindowEventSystem(fabric::async::async_bus_t* app_bus)
          : app_bus_(app_bus) {}

      void run() override {
        ZoneScopedN("WindowEventSystem");
        std::scoped_lock lk{get_instance().window_map_mtx_};
        window_events::poll_events(app_bus_, &get_instance().window_map_);
      }
    private:
      fabric::async::async_bus_t* app_bus_;
    };

    Application()
        : stop_application_listener_(on_event([&](const StopApplicationEvent&) {
            if (this->window_map_.empty()) {
              this->emit<fabric::async::StopBusEvent>();
            }
            this->stop_application_flag_.test_and_set();
            this->stop_application_flag_.notify_all();
          })) {
      ZoneScopedN("Application{}");
      std::latch application_initialization_latch {1};
      using namespace std::chrono_literals;
      add_system<WindowEventSystem>({.enabled = true, .period = 16ms}, this);

      add_init([&] {
        tracy::SetThreadNameWithHint("Application", 1);
        ZoneScopedN("Application Init");
        SDL_SetMainReady();
        if (not SDL_Init(SDL_INIT_VIDEO)) {
          SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
          LOG::print{ERROR}("Couldn't initialize SDL3: {}", SDL_GetError());
        }
        LOG::print {INFO}("SDL3 initialized.");
        application_initialization_latch.count_down();
      });

      start();
      application_initialization_latch.wait();
    }

  public:
    ~Application() {
      ZoneScopedN("~Application");
      stop();
    }

  public:
    static Application &get_instance() {
      static Application instance;
      return instance;
    }

    template <typename... Args>
    static void run(auto&& fun, Args&&... args) {
      ZoneScopedN("Application:run");
      std::latch completion_latch {1};
      get_instance().coroutine_enqueue([&](Args... argss) -> fabric::async::async<bool> {
        ZoneScopedN("Application:run:()");
        fun(std::forward<Args>(argss)...);
        completion_latch.count_down();
        co_return true;
      }, std::forward<Args>(args)...);
      completion_latch.wait();
    }

    template <typename... Args>
    static void run_async(auto&& fun, Args&&... args) {
      ZoneScopedN("Application:run_async");
      get_instance().coroutine_enqueue([&](auto funn, Args... argss) -> fabric::async::async<bool> {
        ZoneScopedN("Application:run_async:()");
        funn(std::forward<Args>(argss)...);
        co_return true;
      }, fun, std::forward<Args>(args)...);
    }

    static void register_window(std::size_t id, fabric::async::async_bus_t* window) {
      ZoneScopedN("Application:register_window");
      std::scoped_lock lk{get_instance().window_map_mtx_};
      get_instance().window_map_[id] = window;
      LOG::print {INFO}("Window registered ID={}", id);
    }
    static void unregister_window(std::size_t id) {
      ZoneScopedN("Application:unregister_window");
      Application& instance = get_instance();
      std::scoped_lock lk{instance.window_map_mtx_};
      instance.window_map_.erase(id);
      if (instance.window_map_.empty() and instance.stop_application_flag_.test()) {
        instance.stop_application_flag_.clear();
        instance.emit<fabric::async::StopBusEvent>();
      }
      LOG::print {INFO}("Window unregistered ID={}", id);
    }

    static void terminate() {
      get_instance().stop();
    }
  private:
    TracyLockable(std::mutex, window_map_mtx_);
    std::map<std::size_t, fabric::async::async_bus_t*> window_map_{};
    fabric::async::listener<StopApplicationEvent>      stop_application_listener_;
    std::atomic_flag                                   stop_application_flag_{false};
  };
}

// using cyd::fabric::async::async;