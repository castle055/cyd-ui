// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include <cyd_fabric_modules/headers/macros/async_events.h>
#include <tracy/Tracy.hpp>
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

export module cydui.application;

export import std;

export import fabric.async;
export import fabric.logging;

export import cydui.window_events;

export namespace cyd::ui {
  class Application: public fabric::async::async_bus_t {
  private:
    Application() {
      std::latch application_initialization_latch {1};
      add_system([this] {
        std::scoped_lock lk {get_instance().window_map_mtx_};
        window_events::poll_events(&get_instance().window_map_);
      });

      add_init([&] {
        SDL_SetMainReady();
        if (0 != SDL_Init(SDL_INIT_VIDEO)) {
          SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        }
        LOG::print {INFO}("SDL2 initialized.");
        application_initialization_latch.count_down();
      });

      start();
      application_initialization_latch.wait();
    }

  public:
    ~Application() {
      stop();
    }

  public:
    static Application &get_instance() {
      static Application instance;
      return instance;
    }

    template <typename... Args>
    static void run(auto&& fun, Args&&... args) {
      std::latch completion_latch {1};
      get_instance().coroutine_enqueue([&](Args... argss) -> fabric::async::async<bool> {
        fun(std::forward<Args>(argss)...);
        completion_latch.count_down();
        co_return true;
      }, std::forward<Args>(args)...);
      completion_latch.wait();
    }

    template <typename... Args>
    static void run_async(auto&& fun, Args&&... args) {
      get_instance().coroutine_enqueue([&](Args... argss) -> fabric::async::async<bool> {
        fun(std::forward<Args>(argss)...);
        co_return true;
      }, std::forward<Args>(args)...);
    }

    static void register_window(std::size_t id, fabric::async::async_bus_t* window) {
      std::scoped_lock lk{get_instance().window_map_mtx_};
      get_instance().window_map_[id] = window;
      LOG::print {INFO}("Window registered ID={}", id);
    }
    static void unregister_window(std::size_t id) {
      std::scoped_lock lk{get_instance().window_map_mtx_};
      get_instance().window_map_.erase(id);
      LOG::print {INFO}("Window unregistered ID={}", id);
    }
  private:
    std::mutex window_map_mtx_{};
    std::map<std::size_t, fabric::async::async_bus_t*> window_map_{};
  };
}

// using cyd::fabric::async::async;