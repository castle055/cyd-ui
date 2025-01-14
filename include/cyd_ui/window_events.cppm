/*! \file  window_events.cppm
 *! \brief 
 *!
 */
module;
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

export module cydui.window_events;

import std;
import reflect;
import fabric.logging;

import fabric.async;

export import cydui.events;

namespace cyd::ui::window_events {
  std::mutex window_map_mutex{};
  std::unordered_map<Uint32, fabric::async::async_bus_t::wptr> window_map{};

  export void add_window(Uint32 id, fabric::async::async_bus_t::wptr window) {
    std::scoped_lock lock{window_map_mutex};
    if (window_map.contains(id)) {
      window_map.erase(id);
    }
    window_map.emplace(id, std::move(window));
  }
  export void remove_window(Uint32 id) {
    std::scoped_lock lock{window_map_mutex};
    window_map.erase(id);
  }
}

namespace cyd::ui::window_events::thread {
  std::unique_ptr<std::thread> thread_ptr{nullptr};
  std::atomic_flag running{};

  template<fabric::async::EventType EV>
  bool emit_to_window(Uint32 win_id, EV &&ev) {
    std::scoped_lock lock {window_map_mutex};
    if (window_map.contains(win_id)) {
      auto& win_ref = window_map.at(win_id);
      if (win_ref.expired()) {
        remove_window(win_id);
        return false;
      }

      win_ref.lock()->emit(ev);
      return true;
    }

    return false;
  }

  void dispatch_window_event(const SDL_WindowEvent &event) {
    switch (event.event) {
      case SDL_WINDOWEVENT_RESIZED:
        LOG::print {INFO}("Window resized to {}x{}", event.data1, event.data2);
        emit_to_window(event.windowID, ResizeEvent {
          .w = event.data1,
          .h = event.data2,
        });
        break;
      default: break;
    }
    if (event.type == SDL_QUIT) {
    }
  }

  void dispatch_event(const SDL_Event &event) {
    switch (event.type) {
      case SDL_QUIT:
        break;
      case SDL_WINDOWEVENT:
        dispatch_window_event(event.window);
        break;
      case SDL_KEYDOWN:
        event.key.windowID;
        break;
      default: break;
    }
  }

  void task() {
    SDL_SetMainReady();
    if (0 != SDL_Init(SDL_INIT_EVENTS)) {
      SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
    }
    SDL_Event event;
    while (running.test()) {
      while (SDL_PollEvent(&event)) {
        dispatch_event(event);
      }
    }
  }
}

export namespace cyd::ui::window_events {
  void start_thread_if_needed() {
    thread::running.test_and_set();
    if (thread::thread_ptr == nullptr) {
      thread::thread_ptr = std::make_unique<std::thread>(thread::task);
    }
  }

}