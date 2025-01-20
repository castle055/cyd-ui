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
  std::unique_ptr<std::thread> thread_ptr{nullptr};
  std::atomic_flag running{};

  using window_map = std::map<std::size_t, fabric::async::async_bus_t*>;

  void dispatch_window_event(window_map *busses, const SDL_WindowEvent &event) {
    auto bus = [&](std::size_t id, auto&& ev) {
      if (busses->contains(id)) {
        busses->at(id)->emit(ev);
        return;
      }
      LOG::print {INFO}("Received event for window {}, but it does not exits", id);
    };

    switch (event.event) {
      case SDL_WINDOWEVENT_RESIZED:
        bus(event.windowID, ResizeEvent {
          .w = event.data1,
          .h = event.data2,
        });
        break;
      case SDL_WINDOWEVENT_CLOSE:
        LOG::print {INFO}("Closing...");
        std::exit(0);
        break;
      case SDL_WINDOWEVENT_EXPOSED:
        bus(event.windowID, RedrawEvent { });
        break;
      case SDL_WINDOWEVENT_LEAVE:
        bus(event.windowID, MotionEvent {
          .x = -1,
          .y = -1,
        });
        break;
      default: break;
    }
    if (event.type == SDL_QUIT) {
    }
  }

  void dispatch_display_event(window_map *busses, const SDL_DisplayEvent &event) {
    auto bus = [&](std::size_t id, auto&& ev) {
      if (busses->contains(id)) {
        busses->at(id)->emit(ev);
        return;
      }
      LOG::print {INFO}("Received event for window {}, but it does not exits", id);
    };

    switch (event.event) {
      case SDL_DISPLAYEVENT_ORIENTATION:
        break;
      case SDL_DISPLAYEVENT_CONNECTED:
        break;
      case SDL_DISPLAYEVENT_DISCONNECTED:
        break;
      case SDL_DISPLAYEVENT_MOVED:
        break;
      default: break;
    }
    if (event.type == SDL_QUIT) {
    }
  }

  void dispatch_event(window_map *busses, const SDL_Event &event) {
    auto bus = [&](std::size_t id, auto&& ev) {
      if (busses->contains(id)) {
        busses->at(id)->emit(ev);
        return;
      }
      LOG::print {INFO}("Received event for window {}, but it does not exits", id);
    };

    switch (event.type) {
      case SDL_QUIT:
        break;
      case SDL_WINDOWEVENT:
        dispatch_window_event(busses, event.window);
        break;
      case SDL_DISPLAYEVENT:
        dispatch_display_event(busses, event.display);
        break;
      case SDL_SYSWMEVENT:
        break;
      case SDL_RENDER_TARGETS_RESET:
      case SDL_RENDER_DEVICE_RESET:
        break;
      case SDL_MOUSEBUTTONDOWN:
      case SDL_MOUSEBUTTONUP:
        bus(event.button.windowID, ButtonEvent {
          .button = event.button.button,
          .x = event.button.x,
          .y = event.button.y,
          .pressed = event.button.state == SDL_PRESSED,
          .released = event.button.state == SDL_RELEASED,
        });
        break;
      case SDL_MOUSEWHEEL:
        bus(event.wheel.windowID, ScrollEvent {
          .dy = event.wheel.preciseY * (event.wheel.direction == SDL_MOUSEWHEEL_FLIPPED? -1: 1),
          .dx = event.wheel.preciseX * (event.wheel.direction == SDL_MOUSEWHEEL_FLIPPED? -1: 1),
          .x = event.wheel.mouseX,
          .y = event.wheel.mouseY
        });
        break;
      case SDL_MOUSEMOTION:
        bus(event.motion.windowID, MotionEvent {
          .x = event.motion.x,
          .y = event.motion.y,
        });
        break;
      case SDL_FINGERDOWN:
      case SDL_FINGERUP:
      case SDL_FINGERMOTION:
        break;
      case SDL_KEYDOWN:
      case SDL_KEYUP:
        bus(event.key.windowID, KeyEvent {
          .keysym = {
            .scancode = event.key.keysym.scancode,
            .code = event.key.keysym.sym,
            .mod = event.key.keysym.mod
          },
          .pressed = event.key.state == SDL_PRESSED,
          .released = event.key.state == SDL_RELEASED,
        });
        break;
      case SDL_TEXTINPUT:
        bus(event.text.windowID, TextInputEvent {
          .text = event.text.text,
        });
        break;
      case SDL_TEXTEDITING:
        bus(event.edit.windowID, TextInputEvent {
          .text = std::string {event.edit.text, static_cast<std::size_t>(event.edit.length)},
          .compositing_event = true,
          .compositing_state = {
            .cursor = event.edit.start,
            .selection = event.edit.length,
          }
        });
        break;
      case SDL_TEXTEDITING_EXT:
        bus(event.editExt.windowID, TextInputEvent {
          .text = std::string {event.editExt.text, static_cast<std::size_t>(event.edit.length)},
          .compositing_event = true,
          .compositing_state = {
            .cursor = event.editExt.start,
            .selection = event.editExt.length,
          }
        });
        SDL_free(event.editExt.text);
        break;
      case SDL_KEYMAPCHANGED:
        break;
      case SDL_DROPFILE:
      case SDL_DROPTEXT:
      case SDL_DROPBEGIN:
      case SDL_DROPCOMPLETE:
        break;
      default: break;
    }
  }

  void task(window_map *busses) {
    ZoneScopedN("Polling events");
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      dispatch_event(busses, event);
    }
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
  void poll_events(window_map *busses) {
    task(busses);
  }
}