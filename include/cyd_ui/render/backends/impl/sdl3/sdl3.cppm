/*! \file  sdl3.cppm
 *! \brief
 *!
 */

module;
#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

export module cydui.backends.sdl3;
export import :dev_texture;
export import :frame_map;
export import :frame;
export import :renderer;
export import :window_events;
export import :event_polling;

import std;
export import reflect;
import fabric.logging;

export import cydui.backends.backend_base;

namespace cydui::backends {
  export class SDL3_backend: public backend_base {
    frame_map::sptr                       registered_frame_map_{std::make_shared<frame_map>()};
    std::unique_ptr<event_polling_system> event_polling_system_{nullptr};

  public:
    using frame_type = SDL3_frame;

    SDL3_backend() {
      SDL_SetMainReady();
      if (not SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        LOG::print{ERROR}("Couldn't initialize SDL3: {}", SDL_GetError());
      }
      event_polling_system_ = std::make_unique<event_polling_system>(registered_frame_map_);
      LOG::print{INFO}("SDL3 initialized.");
    }

    ~SDL3_backend() override {}

    frame_base::sptr make_frame(const std::string& title, int width, int height) override {
      auto frame = std::make_shared<SDL3_frame>(registered_frame_map_, title, width, height);
      registered_frame_map_->register_frame(frame->get_id(), frame);

      LOG::print{INFO}("SDL3 Window initialized.");

      return frame;
    }

  private:
  };
} // namespace cydui::backends
