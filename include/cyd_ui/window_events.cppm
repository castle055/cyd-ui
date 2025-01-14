/*! \file  window_events.cppm
 *! \brief 
 *!
 */
module;
#include <tracy/Tracy.hpp>
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

  void dispatch_window_event(fabric::async::async_bus_t &bus, const SDL_WindowEvent &event) {
    switch (event.event) {
      case SDL_WINDOWEVENT_RESIZED:
        bus.emit(ResizeEvent {
          .w = event.data1,
          .h = event.data2,
        });
        break;
      case SDL_WINDOWEVENT_CLOSE:
        LOG::print {INFO}("Closing...");
        std::exit(0);
        break;
      case SDL_WINDOWEVENT_EXPOSED:
        bus.emit(RedrawEvent {});
        break;
      case SDL_WINDOWEVENT_LEAVE:
        bus.emit(MotionEvent {
          .x = -1,
          .y = -1,
        });
        break;
      default: break;
    }
    if (event.type == SDL_QUIT) {
    }
  }

  void dispatch_event(fabric::async::async_bus_t &bus, const SDL_Event &event) {
    switch (event.type) {
      case SDL_QUIT:
        break;
      case SDL_WINDOWEVENT:
        dispatch_window_event(bus, event.window);
        break;
      case SDL_KEYDOWN:
      case SDL_KEYUP:
        bus.emit(KeyEvent {
          .pressed = event.key.state == SDL_PRESSED,
          .released = event.key.state == SDL_RELEASED,
        });
        break;
      case SDL_MOUSEBUTTONDOWN:
      case SDL_MOUSEBUTTONUP:
        bus.emit(ButtonEvent {
          .button = event.button.button,
          .x = event.button.x,
          .y = event.button.y,
          .pressed = event.button.state == SDL_PRESSED,
          .released = event.button.state == SDL_RELEASED,
        });
        break;
      case SDL_MOUSEMOTION:
        bus.emit(MotionEvent {
          .x = event.motion.x,
          .y = event.motion.y,
        });
        break;
      default: break;
    }
  }

  void task(fabric::async::async_bus_t &bus) {
    ZoneScopedN("Polling events");
    SDL_Event event;
    // while (running.test()) {
      while (SDL_PollEvent(&event)) {
        dispatch_event(bus, event);
      }
    // }
  }
}

export namespace cyd::ui::window_events {
  // void start_thread_if_needed() {
  //   thread::running.test_and_set();
  //   if (thread::thread_ptr == nullptr) {
  //     thread::thread_ptr = std::make_unique<std::thread>(thread::task);
  //   }
  // }
  //
  void poll_events(fabric::async::async_bus_t &bus) {
    thread::task(bus);
  }
}