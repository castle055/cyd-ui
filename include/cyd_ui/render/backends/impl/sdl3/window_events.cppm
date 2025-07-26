/*! \file  window_events.cppm
 *! \brief
 *!
 */
module;
#include "cyd_ui/debug/profiling/macros.h"
#define PROF_CURRENT_MODULE cydui::backends::sdl3::p_window_events

#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>

export module cydui.backends.sdl3:window_events;
import :frame;

import std;
import reflect;
import reflect.serialize;
import reflect.marshal.formats.json;
import fabric.logging;
import cydui.debug.profiling;

import fabric.async;

export import cydui.events;
export import cydui.application;


namespace cydui::backends {
  std::unique_ptr<std::thread> thread_ptr{nullptr};
  std::atomic_flag             running{};

  struct {
    std::unordered_map<std::size_t, std::pair<float, float>> accs{};

    void reset() {
      accs.clear();
    }

    void accumulate(
      const std::size_t bus_id,
      const float       x,
      const float       y
    ) {
      if (not accs.contains(bus_id)) {
        accs[bus_id] = {x, y};
      } else {
        auto& [ax, ay] = accs.at(bus_id);
        ax             = x;
        ay             = y;
      }
    }

    void dispatch(const frame_map::sptr& busses) {
      auto bus = [&](std::size_t id, auto&& ev) {
        PROF_SCOPE(Dispatch Event)
        if (busses->contains(id)) {
          PROF_MESSAGE("EV: {}", refl::serializer<formats::json_fmt>::to_string(ev))
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
    struct scroll_data_t {
      dimensions::screen_measure dy = 0;
      dimensions::screen_measure dx = 0;
      dimensions::screen_measure x  = 0;
      dimensions::screen_measure y  = 0;
    };
    std::unordered_map<std::size_t, scroll_data_t> accs{};

    void reset() {
      accs.clear();
    }

    void accumulate(
      const std::size_t bus_id,
      const float       x,
      const float       y,
      const float       dx,
      const float       dy
    ) {
      if (not accs.contains(bus_id)) {
        accs[bus_id] = {dy, dx, x, y};
      } else {
        auto& data = accs.at(bus_id);
        data.x     = x;
        data.y     = y;
        data.dx    = data.dx + dimensions::screen_measure{dx};
        data.dy    = data.dy + dimensions::screen_measure{dy};
      }
    }

    void dispatch(const frame_map::sptr& busses) {
      auto bus = [&](std::size_t id, auto&& ev) {
        PROF_SCOPE(Dispatch Event)
        if (busses->contains(id)) {
          PROF_MESSAGE("EV: {}", refl::serializer<formats::json_fmt>::to_string(ev))
          busses->at(id)->emit(ev);
          return;
        }
        LOG::print{INFO}("Received event for window {}, but it does not exit", id);
      };
      for (const auto& [id, scrolldata]: accs) {
        bus(
          id,
          ScrollEvent{
            .dy = scrolldata.dy,
            .dx = scrolldata.dx,
            .x  = scrolldata.x,
            .y  = scrolldata.y,
          }
        );
      }
    }
  } scroll_accumulator{};

  struct {
    std::unordered_map<std::size_t, std::pair<float, float>> accs{};

    void reset() {
      accs.clear();
    }

    void accumulate(
      const std::size_t bus_id,
      const float       w,
      const float       h
    ) {
      if (not accs.contains(bus_id)) {
        accs[bus_id] = {w, h};
      } else {
        auto& [aw, ah] = accs.at(bus_id);
        aw             = w;
        ah             = h;
      }
    }

    void dispatch(const frame_map::sptr& busses) {
      auto bus = [&](std::size_t id, auto&& ev) {
        PROF_SCOPE(Dispatch Event)
        if (busses->contains(id)) {
          PROF_MESSAGE("EV: {}", refl::serializer<formats::json_fmt>::to_string(ev))
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

  struct {
    std::unordered_set<std::size_t> accs{};

    void reset() {
      accs.clear();
    }

    void accumulate(const std::size_t bus_id) {
      accs.insert(bus_id);
    }

    void dispatch(const frame_map::sptr& busses) {
      auto bus = [&](std::size_t id, auto&& ev) {
        if (busses->contains(id)) {
          busses->at(id)->emit(ev);
          return;
        }
        LOG::print{INFO}("Received event for window {}, but it does not exit", id);
      };
      for (const auto& id: accs) {
        bus(id, RedrawEvent{});
      }
    }
  } redraw_accumulator{};

  void dispatch_window_event(
    const frame_map::sptr& busses,
    const SDL_WindowEvent& event
  ) {
    auto bus = [&](std::size_t id, auto&& ev) {
      PROF_SCOPE(Dispatch Event)
      if (busses->contains(id)) {
        PROF_MESSAGE("EV: {}", refl::serializer<formats::json_fmt>::to_string(ev))
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
        redraw_accumulator.accumulate(event.windowID);
        break;
      case SDL_EVENT_WINDOW_MOUSE_ENTER:
        break;
      case SDL_EVENT_WINDOW_MOUSE_LEAVE:
        motion_accumulator.accumulate(event.windowID, -1, -1);
        break;
      default:
        break;
    }
  }

  void dispatch_display_event(
    const frame_map::sptr&  busses,
    const SDL_DisplayEvent& event
  ) {
    auto bus = [&](std::size_t id, auto&& ev) {
      PROF_SCOPE(Dispatch Event)
      if (busses->contains(id)) {
        PROF_SCOPE_TEXT("EV: {}", refl::serializer<formats::json_fmt>::to_string(ev))
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

  void dispatch_event(
    const frame_map::sptr& busses,
    const SDL_Event&       event
  ) {
    auto bus = [&](std::size_t id, auto&& ev) {
      PROF_SCOPE(Dispatch Event)
      if (busses->contains(id)) {
        PROF_MESSAGE("EV: {}", refl::serializer<formats::json_fmt>::to_string(ev))
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
          Application::get_instance().emit<StopApplicationEvent>();
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
          scroll_accumulator.accumulate(
            event.wheel.windowID,
            event.wheel.mouse_x,
            event.wheel.mouse_y,
            12 * event.wheel.x * (event.wheel.direction == SDL_MOUSEWHEEL_FLIPPED ? -1 : 1),
            12 * event.wheel.y * (event.wheel.direction == SDL_MOUSEWHEEL_FLIPPED ? -1 : 1)
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

  void task(const frame_map::sptr& busses) {
    PROF_SCOPE(Polling Events);

    redraw_accumulator.reset();
    motion_accumulator.reset();
    scroll_accumulator.reset();
    resize_accumulator.reset();

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      dispatch_event(busses, event);
    }

    motion_accumulator.dispatch(busses);
    scroll_accumulator.dispatch(busses);
    resize_accumulator.dispatch(busses);
    redraw_accumulator.dispatch(busses);
  }
} // namespace cydui::backends

export namespace cydui::backends {
  void poll_events(const frame_map::sptr& busses) {
    task(busses);
  }
} // namespace cydui::backends
