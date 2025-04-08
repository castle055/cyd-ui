/*! \file  window_events.cppm
 *! \brief
 *!
 */
module;
#include <cyd_fabric_modules/headers/macros/async_events.h>
#include <tracy/Tracy.hpp>
#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>

export module cydui.window_events;

import std;
import reflect;
import fabric.logging;

import fabric.async;

export import cydui.events;


export namespace cydui {
  EVENT(StopApplicationEvent){};
}

namespace cydui::window_events {
  std::unique_ptr<std::thread> thread_ptr{nullptr};
  std::atomic_flag             running{};

  using window_map = std::map<std::size_t, fabric::async::async_bus_t*>;

  struct {
    std::unordered_map<std::size_t, std::pair<float, float>> accs{};

    void reset() {
      accs.clear();
    }

    void accumulate(const std::size_t bus_id, const float x, const float y) {
      if (not accs.contains(bus_id)) {
        accs[bus_id] = {x, y};
      } else {
        auto& [ax, ay] = accs.at(bus_id);
        ax = x;
        ay = y;
      }
    }

    void dispatch(window_map* busses) {
      auto bus = [&](std::size_t id, auto&& ev) {
        if (busses->contains(id)) {
          busses->at(id)->emit(ev);
          return;
        }
        LOG::print{INFO}("Received event for window {}, but it does not exit", id);
      };
      for (const auto& [id, motion]: accs) {
        bus(
          id,
          MotionEvent{
            .x = motion.first,
            .y = motion.second,
          }
        );
      }
    }
  } motion_accumulator{};

  struct {
    std::unordered_map<std::size_t, std::pair<float, float>> accs{};

    void reset() {
      accs.clear();
    }

    void accumulate(const std::size_t bus_id, const float w, const float h) {
      if (not accs.contains(bus_id)) {
        accs[bus_id] = {w, h};
      } else {
        auto& [aw, ah] = accs.at(bus_id);
        aw = w;
        ah = h;
      }
    }

    void dispatch(window_map* busses) {
      auto bus = [&](std::size_t id, auto&& ev) {
        if (busses->contains(id)) {
          busses->at(id)->emit(ev);
          return;
        }
        LOG::print{INFO}("Received event for window {}, but it does not exit", id);
      };
      for (const auto& [id, size]: accs) {
        bus(
          id,
          ResizeEvent{
            .w = size.first,
            .h = size.second,
          }
        );
      }
    }
  } resize_accumulator{};

  void dispatch_window_event(window_map* busses, const SDL_WindowEvent& event) {
    auto bus = [&](std::size_t id, auto&& ev) {
      if (busses->contains(id)) {
        busses->at(id)->emit(ev);
        return;
      }
      LOG::print{INFO}("Received event for window {}, but it does not exit", id);
    };

    switch (event.type) {
      case SDL_EVENT_WINDOW_RESIZED:
        resize_accumulator.accumulate(event.windowID, event.data1, event.data2);
        break;
      case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
        LOG::print{INFO}("Closing...");
        bus(event.windowID, WindowClosed{});
        break;
      case SDL_EVENT_WINDOW_EXPOSED:
        bus(event.windowID, RedrawEvent{});
        break;
      case SDL_EVENT_WINDOW_MOUSE_ENTER:
        break;
      case SDL_EVENT_WINDOW_MOUSE_LEAVE:
        bus(
          event.windowID,
          MotionEvent{
            .x = -1,
            .y = -1,
          }
        );
        break;
      default:
        break;
    }
  }

  void dispatch_display_event(window_map* busses, const SDL_DisplayEvent& event) {
    auto bus = [&](std::size_t id, auto&& ev) {
      if (busses->contains(id)) {
        busses->at(id)->emit(ev);
        return;
      }
      LOG::print{INFO}("Received event for window {}, but it does not exits", id);
    };

    switch (event.type) {
      case SDL_EVENT_DISPLAY_ORIENTATION:
        break;
      case SDL_EVENT_DISPLAY_ADDED:
        break;
      case SDL_EVENT_DISPLAY_REMOVED:
        break;
      case SDL_EVENT_DISPLAY_MOVED:
        break;
      default:
        break;
    }
  }

  void
  dispatch_event(fabric::async::async_bus_t* app_bus, window_map* busses, const SDL_Event& event) {
    auto bus = [&](std::size_t id, auto&& ev) {
      if (busses->contains(id)) {
        busses->at(id)->emit(ev);
        return;
      }
      LOG::print{INFO}("Received event for window {}, but it does not exits", id);
    };

    if (event.type >= SDL_EVENT_WINDOW_FIRST and event.type <= SDL_EVENT_WINDOW_LAST) {
      dispatch_window_event(busses, event.window);
    } else if (event.type >= SDL_EVENT_DISPLAY_FIRST and event.type <= SDL_EVENT_DISPLAY_LAST) {
      dispatch_display_event(busses, event.display);
    } else {
      switch (event.type) {
        case SDL_EVENT_QUIT:
          app_bus->emit<StopApplicationEvent>();
          break;
        case SDL_EVENT_RENDER_TARGETS_RESET:
        case SDL_EVENT_RENDER_DEVICE_RESET:
          break;
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        case SDL_EVENT_MOUSE_BUTTON_UP:
          bus(
            event.button.windowID,
            ButtonEvent{
              .button   = event.button.button,
              .x        = event.button.x,
              .y        = event.button.y,
              .pressed  = event.button.down,
              .released = not event.button.down,
            }
          );
          break;
        case SDL_EVENT_MOUSE_WHEEL:
          bus(
            event.wheel.windowID,
            ScrollEvent{
              .dy = event.wheel.y * (event.wheel.direction == SDL_MOUSEWHEEL_FLIPPED ? -1 : 1),
              .dx = event.wheel.x * (event.wheel.direction == SDL_MOUSEWHEEL_FLIPPED ? -1 : 1),
              .x  = event.wheel.mouse_x,
              .y  = event.wheel.mouse_y
            }
          );
          break;
        case SDL_EVENT_MOUSE_MOTION:
          motion_accumulator.accumulate(event.motion.windowID, event.motion.x, event.motion.y);
          break;
        case SDL_EVENT_FINGER_DOWN:
        case SDL_EVENT_FINGER_UP:
        case SDL_EVENT_FINGER_MOTION:
          break;
        case SDL_EVENT_KEY_DOWN:
        case SDL_EVENT_KEY_UP:
          bus(
            event.key.windowID,
            KeyEvent{
              .keysym =
                {.scancode = event.key.scancode, .code = event.key.key, .mod = event.key.mod},
              .pressed  = event.key.down,
              .released = not event.key.down,
            }
          );
          break;
        case SDL_EVENT_TEXT_INPUT:
          bus(
            event.text.windowID,
            TextInputEvent{
              .text = event.text.text,
            }
          );
          break;
        case SDL_EVENT_TEXT_EDITING:
          bus(
            event.edit.windowID,
            TextInputEvent{
              .text = std::string{event.edit.text, static_cast<std::size_t>(event.edit.length)},
              .compositing_event = true,
              .compositing_state =
                {
                  .cursor    = event.edit.start,
                  .selection = event.edit.length,
                }
            }
          );
          break;
        case SDL_EVENT_TEXT_EDITING_CANDIDATES:
          // TODO
          // bus(
          //   event.edit_candidates.windowID,
          //   TextInputEvent{
          //     .text = std::string{event.edit_candidates.,
          //     static_cast<std::size_t>(event.edit.length)}, .compositing_event = true,
          //     .compositing_state =
          //       {
          //         .cursor    = event.edit_candidates.start,
          //         .selection = event.edit_candidates.length,
          //       }
          //   }
          // );
          // SDL_free(event.editExt.text);
          break;
        case SDL_EVENT_KEYMAP_CHANGED:
          break;
        case SDL_EVENT_DROP_FILE:
        case SDL_EVENT_DROP_TEXT:
        case SDL_EVENT_DROP_BEGIN:
        case SDL_EVENT_DROP_COMPLETE:
          break;
        default:
          break;
      }
    }
  }

  void task(fabric::async::async_bus_t* app_bus, window_map* busses) {
    ZoneScopedN("Polling events");
    motion_accumulator.reset();
    resize_accumulator.reset();

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      dispatch_event(app_bus, busses, event);
    }

    motion_accumulator.dispatch(busses);
    resize_accumulator.dispatch(busses);
  }
} // namespace cydui::window_events

export namespace cydui::window_events {
  // void start_thread_if_needed() {
  //   thread::running.test_and_set();
  //   if (thread::thread_ptr == nullptr) {
  //     thread::thread_ptr = std::make_unique<std::thread>(thread::task);
  //   }
  // }
  //
  void poll_events(fabric::async::async_bus_t* app_bus, window_map* busses) {
    task(app_bus, busses);
  }
} // namespace cydui::window_events
