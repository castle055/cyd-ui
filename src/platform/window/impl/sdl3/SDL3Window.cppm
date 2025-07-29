/*! \file  SDL3Window.cppm
 *! \brief
 *!
 */

module;
#include <tracy/Tracy.hpp>
#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>

#include "../../../../debug/profiling/macros.h"
#define PROF_CURRENT_MODULE cydui::platform::window::sdl3::window

export module cydui.platform.window.sdl3.window;

import std;
import reflect;

export import cydui.platform.window.window_base;
export import cydui.platform.window.sdl3;
export import cydui.platform.window.sdl3.service;
import cydui.debug.profiling;

namespace cydui::platform::window {
  export class SDL3Window final: public WindowBase {
    SDL3Service&      sdl;
    SDL3WindowHandle  window_;
    SDL3WindowOptions options_;

    SDL_Texture* device_buffer_ {nullptr};
    int          old_w {0}, old_h {0};

    explicit SDL3Window(
      SDL3Service&             sdl,
      const SDL3WindowHandle&  handle,
      const SDL3WindowOptions& options)
        : sdl(sdl),
          window_(handle),
          options_(options) {}

  public:
    using sptr = std::shared_ptr<SDL3Window>;

    static fabric::task<sptr> start(
      fabric::services::ServiceLocator& locator,
      const SDL3WindowOptions&          options) {
      auto& sdl = co_await locator.require<SDL3Service>();

      SDL3WindowHandle window_handle = co_await sdl.create_window(options);

      co_return sptr {new SDL3Window(sdl, window_handle, options)};
    }

    static fabric::task<> stop(SDL3Window& self) {
      co_await self.sdl.destroy_window(self.window_);
    }

    SDL3Window(const SDL3Window&)            = delete;
    SDL3Window& operator=(const SDL3Window&) = delete;
    SDL3Window(SDL3Window&&)                 = delete;
    SDL3Window& operator=(SDL3Window&&)      = delete;

    id_type get_id() const override {
      return window_.id;
    }

    fabric::task<> set_position(
      int x,
      int y) override {
      PROF_SCOPE(SDL3::set_position);
      co_await sdl.move_window(window_, x, y);
    }

    fabric::task<> enable_text_input() override {
      PROF_SCOPE(SDL3::enable_text_input);
      co_await sdl.set_window_text_input(window_, true);
    }

    fabric::task<> disable_text_input() override {
      PROF_SCOPE(SDL3::disable_text_input);
      co_await sdl.set_window_text_input(window_, false);
    }

    fabric::task<std::pair<
      int,
      int>>
    get_size() override {
      PROF_SCOPE(SDL3::get_size);
      co_return co_await sdl.get_window_size(window_);
    }

    fabric::task<> present(const Surface& surface) override {
      PROF_SCOPE(SDL3::present);
      if (surface.type == SurfaceType::CPU) {
        if (old_w != surface.size.first || old_h != surface.size.second) {
          co_await sdl.destroy_texture(device_buffer_);
          device_buffer_ = nullptr;
        }

        if (device_buffer_ == nullptr) {
          SDL_PixelFormat pixel_format = SDL_PIXELFORMAT_ARGB8888;
          switch (surface.pixel_format) {
            case PixelFormat::RGBA32: pixel_format = SDL_PIXELFORMAT_RGBA8888; break;
            case PixelFormat::ARGB32: pixel_format = SDL_PIXELFORMAT_ARGB8888; break;
          }
          device_buffer_ = co_await sdl.create_texture(
            window_.renderer, pixel_format, SDL_TEXTUREACCESS_STREAMING, surface.size.first, surface.size.second);
        }

        co_await sdl.update_texture(device_buffer_, surface.handle, surface.pitch);

        fabric::launch(
          sdl.present_to_window(
            window_, device_buffer_, static_cast<float>(surface.size.first), static_cast<float>(surface.size.second)))
          .detach();
      }
    }

    fabric::task<> event_task(fabric::async::async_bus_t::sptr bus) override {
      co_await sdl.register_window_bus(window_, bus);
    }
  };
} // namespace cydui::platform::window
