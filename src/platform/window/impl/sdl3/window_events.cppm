/*! \file  window_events.cppm
 *! \brief
 *!
 */
module;
#include "../../../../debug/profiling/macros.h"
#define PROF_CURRENT_MODULE cydui::platform::window::sdl3::window_events

#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>

export module cydui.platform.window.sdl3.window_events;

import std;
import fabric.async;
import fabric.logging;
import reflect;
import reflect.serialize;
import reflect.marshal.formats.json;
import cydui.debug.profiling;
export import cydui.platform.window.window_base;
export import cydui.event_types;

import cydui.platform.window.sdl3.scancode_map;
import cydui.platform.window.sdl3.keycode_map;

export namespace cydui::platform::window {
  using BusMap = std::unordered_map<WindowBase::id_type, fabric::async::async_bus_t::sptr>;
}

namespace cydui::platform::window {
  std::unique_ptr<std::thread> thread_ptr {nullptr};
  std::atomic_flag             running {};

  struct {
    std::unordered_map<std::size_t, std::pair<float, float>> accs {};

    void reset() {
      accs.clear();
    }

    void accumulate(
      const std::size_t bus_id,
      const float       x,
      const float       y) {
      if (not accs.contains(bus_id)) {
        accs[bus_id] = {x, y};
      } else {
        auto& [ax, ay] = accs.at(bus_id);
        ax             = x;
        ay             = y;
      }
    }

    void dispatch(BusMap& busses) {
      auto bus = [&](std::size_t id, auto&& ev) {
        PROF_SCOPE(Dispatch Event)
        if (busses.contains(id)) {
          PROF_MESSAGE("EV: MotionEvent {}", refl::serializer<formats::json_fmt>::to_string(ev))
          busses.at(id)->emit(ev);
          return;
        }
        LOG::print {WARN}("Received event for window {}, but it does not exit", id);
      };
      for (const auto& [id, motion]: accs) {
        bus(
          id,
          MotionEvent {
            .x = motion.first,
            .y = motion.second,
          });
      }
    }
  } motion_accumulator {};

  struct {
    struct scroll_data_t {
      dimensions::screen_measure dy = 0;
      dimensions::screen_measure dx = 0;
      dimensions::screen_measure x  = 0;
      dimensions::screen_measure y  = 0;
    };
    std::unordered_map<std::size_t, scroll_data_t> accs {};

    void reset() {
      accs.clear();
    }

    void accumulate(
      const std::size_t bus_id,
      const float       x,
      const float       y,
      const float       dx,
      const float       dy) {
      if (not accs.contains(bus_id)) {
        accs[bus_id] = {dy, dx, x, y};
      } else {
        auto& data = accs.at(bus_id);
        data.x     = x;
        data.y     = y;
        data.dx    = data.dx + dimensions::screen_measure {dx};
        data.dy    = data.dy + dimensions::screen_measure {dy};
      }
    }

    void dispatch(BusMap& busses) {
      auto bus = [&](std::size_t id, auto&& ev) {
        PROF_SCOPE(Dispatch Event)
        if (busses.contains(id)) {
          PROF_MESSAGE("EV: ScrollEvent {}", refl::serializer<formats::json_fmt>::to_string(ev))
          busses.at(id)->emit(ev);
          return;
        }
        LOG::print {WARN}("Received event for window {}, but it does not exit", id);
      };
      for (const auto& [id, scrolldata]: accs) {
        bus(
          id,
          ScrollEvent {
            .dy = scrolldata.dy,
            .dx = scrolldata.dx,
            .x  = scrolldata.x,
            .y  = scrolldata.y,
          });
      }
    }
  } scroll_accumulator {};

  struct {
    std::unordered_map<std::size_t, std::pair<float, float>> accs {};

    void reset() {
      accs.clear();
    }

    void accumulate(
      const std::size_t bus_id,
      const float       w,
      const float       h) {
      if (not accs.contains(bus_id)) {
        accs[bus_id] = {w, h};
      } else {
        auto& [aw, ah] = accs.at(bus_id);
        aw             = w;
        ah             = h;
      }
    }

    void dispatch(BusMap& busses) {
      auto bus = [&](std::size_t id, auto&& ev) {
        PROF_SCOPE(Dispatch Event)
        if (busses.contains(id)) {
          PROF_MESSAGE("EV: ResizeEvent {}", refl::serializer<formats::json_fmt>::to_string(ev))
          busses.at(id)->emit(ev);
          return;
        }
        LOG::print {WARN}("Received event for window {}, but it does not exit", id);
      };
      for (const auto& [id, size]: accs) {
        bus(
          id,
          ResizeEvent {
            .w = size.first,
            .h = size.second,
          });
      }
    }
  } resize_accumulator {};

  struct {
    std::unordered_set<std::size_t> accs {};

    void reset() {
      accs.clear();
    }

    void accumulate(const std::size_t bus_id) {
      accs.insert(bus_id);
    }

    void dispatch(BusMap& busses) {
      auto bus = [&](std::size_t id, auto&& ev) {
        PROF_SCOPE(Dispatch Event)
        if (busses.contains(id)) {
          PROF_MESSAGE("EV: RedrawEvent {}", refl::serializer<formats::json_fmt>::to_string(ev))
          busses.at(id)->emit(ev);
          return;
        }
        LOG::print {WARN}("Received event for window {}, but it does not exit", id);
      };
      for (const auto& id: accs) {
        bus(id, RedrawEvent {});
      }
    }
  } redraw_accumulator {};

  void dispatch_window_event(
    BusMap&                busses,
    const SDL_WindowEvent& event) {
    auto bus = [&](std::size_t id, auto&& ev) {
      PROF_SCOPE(Dispatch Event)
      if (busses.contains(id)) {
        PROF_MESSAGE("EV: {}", refl::serializer<formats::json_fmt>::to_string(ev))
        busses.at(id)->emit(ev);
        return;
      }
      LOG::print {WARN}("Received event for window {}, but it does not exit", id);
    };

    switch (event.type) {
      case SDL_EVENT_WINDOW_RESIZED: resize_accumulator.accumulate(event.windowID, event.data1, event.data2); break;
      case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
        LOG::print {INFO}("Closing...");
        bus(event.windowID, WindowCloseRequested {});
        break;
      case SDL_EVENT_WINDOW_EXPOSED    : redraw_accumulator.accumulate(event.windowID); break;
      case SDL_EVENT_WINDOW_MOUSE_ENTER: break;
      case SDL_EVENT_WINDOW_MOUSE_LEAVE: motion_accumulator.accumulate(event.windowID, -1, -1); break;
      default                          : break;
    }
  }

  void dispatch_display_event(
    BusMap&                 busses,
    const SDL_DisplayEvent& event) {
    auto bus = [&](std::size_t id, auto&& ev) {
      PROF_SCOPE(Dispatch Event)
      if (busses.contains(id)) {
        PROF_SCOPE_TEXT("EV: {}", refl::serializer<formats::json_fmt>::to_string(ev))
        busses.at(id)->emit(ev);
        return;
      }
      LOG::print {WARN}("Received event for window {}, but it does not exits", id);
    };

    switch (event.type) {
      case SDL_EVENT_DISPLAY_ORIENTATION: break;
      case SDL_EVENT_DISPLAY_ADDED      : break;
      case SDL_EVENT_DISPLAY_REMOVED    : break;
      case SDL_EVENT_DISPLAY_MOVED      : break;
      default                           : break;
    }
  }

  void dispatch_event(
    BusMap&          busses,
    const SDL_Event& event) {
    auto bus = [&](std::size_t id, auto&& ev) {
      PROF_SCOPE(Dispatch Event)
      if (busses.contains(id)) {
        PROF_MESSAGE("EV: {}", refl::serializer<formats::json_fmt>::to_string(ev))
        busses.at(id)->emit(ev);
        return;
      }
      LOG::print {WARN}("Received event for window {}, but it does not exits", id);
    };

    if (event.type >= SDL_EVENT_WINDOW_FIRST and event.type <= SDL_EVENT_WINDOW_LAST) {
      dispatch_window_event(busses, event.window);
    } else if (event.type >= SDL_EVENT_DISPLAY_FIRST and event.type <= SDL_EVENT_DISPLAY_LAST) {
      dispatch_display_event(busses, event.display);
    } else {
      switch (event.type) {
        case SDL_EVENT_QUIT:
          bus(event.button.windowID, WindowCloseRequested {});
          break;
        case SDL_EVENT_RENDER_TARGETS_RESET:
        case SDL_EVENT_RENDER_DEVICE_RESET : break;
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        case SDL_EVENT_MOUSE_BUTTON_UP:
          bus(
            event.button.windowID,
            ButtonEvent {
              .button   = event.button.button,
              .x        = event.button.x,
              .y        = event.button.y,
              .pressed  = event.button.down,
              .released = not event.button.down,
            });
          break;
        case SDL_EVENT_MOUSE_WHEEL:
          scroll_accumulator.accumulate(
            event.wheel.windowID,
            event.wheel.mouse_x,
            event.wheel.mouse_y,
            64 * event.wheel.x * (event.wheel.direction == SDL_MOUSEWHEEL_FLIPPED ? -1 : 1),
            64 * event.wheel.y * (event.wheel.direction == SDL_MOUSEWHEEL_FLIPPED ? -1 : 1));
          break;
        case SDL_EVENT_MOUSE_MOTION:
          motion_accumulator.accumulate(event.motion.windowID, event.motion.x, event.motion.y);
          break;
        case SDL_EVENT_FINGER_DOWN:
        case SDL_EVENT_FINGER_UP:
        case SDL_EVENT_FINGER_MOTION: break;
        case SDL_EVENT_KEY_DOWN:
        case SDL_EVENT_KEY_UP:
          bus(
            event.key.windowID,
            KeyEvent {
              .scancode = map_sdl3_scancode(event.key.scancode),
              .code     = map_sdl3_keycode(event.key.key),
              .modifier = map_sdl3_keymod(event.key.mod),
              .pressed  = event.key.down,
              .released = not event.key.down,
            });
          break;
        case SDL_EVENT_TEXT_INPUT:
          bus(
            event.text.windowID,
            TextInputEvent {
              .text = event.text.text,
            });
          break;
        case SDL_EVENT_TEXT_EDITING:
          bus(
            event.edit.windowID,
            TextInputEvent {
              .text              = std::string {event.edit.text, static_cast<std::size_t>(event.edit.length)},
              .compositing_event = true,
              .compositing_state = {
                                                .cursor    = event.edit.start,
                                                .selection = event.edit.length,
                                                }
          });
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
        case SDL_EVENT_KEYMAP_CHANGED: break;
        case SDL_EVENT_DROP_FILE     :
        case SDL_EVENT_DROP_TEXT     :
        case SDL_EVENT_DROP_BEGIN    :
        case SDL_EVENT_DROP_COMPLETE : break;
        default                      : break;
      }
    }
  }
} // namespace cydui::platform::window

export namespace cydui::platform::window {
  fabric::task<> poll_events(BusMap* busses) {
    PROF_SCOPE(Polling Events);

    {
      PROF_SCOPE(Reset Accumulators);
      redraw_accumulator.reset();
      motion_accumulator.reset();
      scroll_accumulator.reset();
      resize_accumulator.reset();
    }

    SDL_Event events[64];
    {
      PROF_SCOPE(Process Event Queue);
      int n;
      SDL_PumpEvents();
      while ((n = SDL_PeepEvents(events, 64, SDL_GETEVENT, SDL_EVENT_FIRST, SDL_EVENT_LAST)) != 0) {
        PROF_SCOPE(Process Event)
        for (int i = 0; i < n; ++i) {
          dispatch_event(*busses, events[i]);
        }
      }
    }

    {
      PROF_SCOPE(Dispatch Accumulators);
      motion_accumulator.dispatch(*busses);
      scroll_accumulator.dispatch(*busses);
      resize_accumulator.dispatch(*busses);
      redraw_accumulator.dispatch(*busses);
    }
    co_return;
  }
} // namespace cydui::platform::window
