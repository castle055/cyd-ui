/*! \file  SDL3Window.cppm
 *! \brief
 *!
 */

module;
#include <tracy/Tracy.hpp>
#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "../../../../debug/profiling/macros.h"
#define PROF_CURRENT_MODULE cydui::platform::window::sdl3::service

export module cydui.platform.window.sdl3.service;

import std;
import reflect;

export import fabric.services;

export import cydui.platform.window.window_base;
export import cydui.platform.window.sdl3;
export import cydui.platform.window.sdl3.window_events;
import cydui.debug.profiling;

namespace cydui::platform::window {
  export struct SDL3WindowHandle {
    WindowType          type;
    WindowBase::id_type id;
    SDL_Window*         window;
    SDL_Renderer*       renderer;
  };

  export class SDL3Service final: public fabric::services::ServiceBase {
    fabric::tasks::executor::sptr executor_;

    std::stop_source                     stop_source_;
    std::optional<fabric::shared_task<>> event_polling_task_;

    BusMap registered_windows_ {};

    explicit SDL3Service(const fabric::tasks::executor::sptr& executor)
        : executor_(executor) {
      event_polling_task_ =
        executor_->schedule(event_polling_task(stop_source_.get_token(), &registered_windows_)).share();
      LOG::print {INFO}("SDL3 initialized.");
    }

    static fabric::task<> event_polling_task(
      std::stop_token stop_token,
      BusMap*         window_map) {
      using namespace std::chrono_literals;
      while (not stop_token.stop_requested()) {
        if (not window_map->empty()) {
          co_await poll_events(window_map);
        }
        co_await 16ms;
      }
    }

  public:
    using scope = services::GlobalScope;

    static fabric::task<sptr> start(fabric::services::ServiceLocator& locator) {
      PROF_SCOPE(SDL3Service START);
      auto exec = co_await fabric::this_task::get_executor_sptr();

      SDL_SetMainReady();
      // SDL_SetHint(SDL_HINT_RENDER_DRIVER, "software");
      // SDL_SetHint(SDL_HINT_VIDEO_DRIVER, "x11");
      if (not SDL_Init(SDL_INIT_VIDEO)) {
        LOG::print {ERROR}("Couldn't initialize SDL3: {}", SDL_GetError());
      }

      co_return sptr {new SDL3Service(exec)};
    }

    static fabric::task<> stop(SDL3Service& self) {
      PROF_SCOPE(SDL3Service STOP);
      self.stop_source_.request_stop();
      SDL_Quit();
      // if (self.event_polling_task_.has_value()) {
      // co_await self.event_polling_task_.value();
      // }
      co_return;
    }

    ~SDL3Service() override {}

    fabric::task<SDL3WindowHandle> create_window(const SDL3WindowOptions& options) const {
      co_await fabric::this_task::switch_executor(executor_);
      PROF_SCOPE(SDL3::create_window);

      SDL3WindowHandle handle {};
      handle.type = WindowType::TOPLEVEL;

      SDL_SetHint(SDL_HINT_X11_FORCE_OVERRIDE_REDIRECT, options.x11_override_redirect ? "1" : "0");
      SDL_SetHint(SDL_HINT_X11_WINDOW_TYPE, "_NET_WM_WINDOW_TYPE_NORMAL");
      if (not SDL_CreateWindowAndRenderer(
            options.title.c_str(),
            options.width,
            options.height,
            SDL_WINDOW_RESIZABLE | SDL_WINDOW_TRANSPARENT,
            &handle.window,
            &handle.renderer)) {
        throw fabric::exception {std::format("Couldn't create SDL window: {}", SDL_GetError())};
      }

      handle.id = SDL_GetWindowID(handle.window);

      co_return handle;
    }

    fabric::task<SDL3WindowHandle> create_modal_window(
      const SDL3WindowHandle&  parent,
      const SDL3WindowOptions& options) const {
      co_await fabric::this_task::switch_executor(executor_);
      PROF_SCOPE(SDL3::create_window);

      SDL3WindowHandle handle {};
      handle.type = WindowType::MODAL;

      SDL_SetHint(SDL_HINT_X11_FORCE_OVERRIDE_REDIRECT, options.x11_override_redirect ? "1" : "0");
      SDL_SetHint(SDL_HINT_X11_WINDOW_TYPE, "_NET_WM_WINDOW_TYPE_DIALOG");
      if (not SDL_CreateWindowAndRenderer(
            options.title.c_str(),
            options.width,
            options.height,
            SDL_WINDOW_RESIZABLE | SDL_WINDOW_TRANSPARENT,
            &handle.window,
            &handle.renderer)) {
        throw fabric::exception {std::format("Couldn't create SDL window: {}", SDL_GetError())};
      }
      SDL_SetWindowParent(handle.window, parent.window);
      SDL_SetWindowModal(handle.window, true);

      handle.id = SDL_GetWindowID(handle.window);

      co_return handle;
    }

    fabric::task<SDL3WindowHandle> create_popup_window(
      const SDL3WindowHandle&  parent,
      const SDL3WindowOptions& options) const {
      co_await fabric::this_task::switch_executor(executor_);
      PROF_SCOPE(SDL3::create_window);

      SDL3WindowHandle handle {};
      handle.type = WindowType::POPUP;

      SDL_SetHint(SDL_HINT_X11_FORCE_OVERRIDE_REDIRECT, options.x11_override_redirect ? "1" : "0");
      SDL_SetHint(SDL_HINT_X11_WINDOW_TYPE, "_NET_WM_WINDOW_TYPE_POPUP_MENU");
      handle.window = SDL_CreatePopupWindow(
        parent.window,
        options.x,
        options.y,
        options.width,
        options.height,
        SDL_WINDOW_ALWAYS_ON_TOP | SDL_WINDOW_POPUP_MENU);

      if (handle.window == nullptr) {
        throw fabric::exception {std::format("Couldn't create SDL window: {}", SDL_GetError())};
      }

      handle.renderer = SDL_CreateRenderer(handle.window, nullptr);
      if (handle.renderer == nullptr) {
        throw fabric::exception {std::format("Couldn't create SDL window renderer: {}", SDL_GetError())};
      }

      handle.id = SDL_GetWindowID(handle.window);

      co_return handle;
    }

    fabric::task<SDL3WindowHandle> create_tooltip_window(
      const SDL3WindowHandle&  parent,
      const SDL3WindowOptions& options) const {
      co_await fabric::this_task::switch_executor(executor_);
      PROF_SCOPE(SDL3::create_window);

      SDL3WindowHandle handle {};
      handle.type = WindowType::POPUP;

      SDL_SetHint(SDL_HINT_X11_FORCE_OVERRIDE_REDIRECT, options.x11_override_redirect ? "1" : "0");
      SDL_SetHint(SDL_HINT_X11_WINDOW_TYPE, "_NET_WM_WINDOW_TYPE_TOOLTIP");
      handle.window = SDL_CreatePopupWindow(
        parent.window,
        options.x,
        options.y,
        options.width,
        options.height,
        SDL_WINDOW_ALWAYS_ON_TOP | SDL_WINDOW_TOOLTIP);

      if (handle.window == nullptr) {
        throw fabric::exception {std::format("Couldn't create SDL window: {}", SDL_GetError())};
      }

      handle.renderer = SDL_CreateRenderer(handle.window, nullptr);
      if (handle.renderer == nullptr) {
        throw fabric::exception {std::format("Couldn't create SDL window renderer: {}", SDL_GetError())};
      }

      handle.id = SDL_GetWindowID(handle.window);

      co_return handle;
    }

    fabric::task<> destroy_window(const SDL3WindowHandle& handle) {
      co_await fabric::this_task::switch_executor(executor_);
      PROF_SCOPE(SDL3::destroy_window);

      co_await unregister_window_bus(handle);
      SDL_DestroyWindow(handle.window);
      SDL_DestroyRenderer(handle.renderer);
    }

    fabric::task<> move_window(
      const SDL3WindowHandle& handle,
      int                     x,
      int                     y) const {
      co_await fabric::this_task::switch_executor(executor_);
      PROF_SCOPE(SDL3::move_window);
      SDL_SetWindowPosition(handle.window, x, y);
    }

    fabric::task<std::pair<
      int,
      int>>
    get_window_size(const SDL3WindowHandle& handle) const {
      co_await fabric::this_task::switch_executor(executor_);
      PROF_SCOPE(SDL3::get_window_size);
      int w, h;
      SDL_GetCurrentRenderOutputSize(handle.renderer, &w, &h);
      // SDL_GetWindowSize(window_, &w, &h);
      co_return {w, h};
    }

    fabric::task<> set_window_text_input(
      const SDL3WindowHandle& handle,
      bool                    enabled) const {
      co_await fabric::this_task::switch_executor(executor_);
      PROF_SCOPE(SDL3::set_window_text_input);
      if (not enabled) {
        SDL_StartTextInput(handle.window);
      } else {
        SDL_StopTextInput(handle.window);
      }
    }

    fabric::task<> present_to_window(
      const SDL3WindowHandle& handle,
      SDL_Texture*            texture,
      float                   w,
      float                   h) const {
      co_await fabric::this_task::switch_executor(executor_);
      PROF_SCOPE(SDL3::present_to_window);

      SDL_FRect src {0.0f, 0.0f, w, h};
      SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_NONE);

      SDL_SetRenderDrawColor(handle.renderer, 0x00, 0x00, 0x00, 0x00);
      SDL_RenderFillRect(handle.renderer, &src);
      SDL_RenderTexture(handle.renderer, texture, &src, &src);
      // SDL_FRect r{.x = 100, .y = 210, .w = 100, .h = 100};
      // SDL_SetRenderDrawColor(renderer_, 0, 0xff, 0xff, 0xff);
      // SDL_RenderFillRect(renderer_, &r);

      // SDL_Surface* thumbnail = read_texture_pixels(texture_ptr, 4);
      // if (nullptr != thumbnail) {
      //   FrameImage(thumbnail->pixels, thumbnail->w, thumbnail->h, 0, false);
      //   SDL_DestroySurface(thumbnail);
      // }
      SDL_RenderPresent(handle.renderer);
    }

    fabric::task<> register_window_bus(
      const SDL3WindowHandle&                 handle,
      const fabric::async::async_bus_t::sptr& bus) {
      co_await fabric::this_task::switch_executor(executor_);
      PROF_SCOPE(SDL3::register_window_bus);
      registered_windows_.emplace(handle.id, bus);
    }

    fabric::task<> unregister_window_bus(const SDL3WindowHandle& handle) {
      co_await fabric::this_task::switch_executor(executor_);
      PROF_SCOPE(SDL3::unregister_window_bus);
      registered_windows_.erase(handle.id);
    }

    fabric::task<SDL_Texture*> create_texture(
      SDL_Renderer*     renderer,
      SDL_PixelFormat   pixel_format,
      SDL_TextureAccess access,
      int               width,
      int               height) {
      co_await fabric::this_task::switch_executor(executor_);
      PROF_SCOPE(SDL3::create_texture);
      co_return SDL_CreateTexture(renderer, pixel_format, access, width, height);
    }

    fabric::task<> update_texture(
      SDL_Texture* texture,
      const void*  data,
      int          pitch) {
      co_await fabric::this_task::switch_executor(executor_);
      PROF_SCOPE(SDL3::update_texture);
      SDL_UpdateTexture(texture, nullptr, data, pitch);
    }

    fabric::task<> destroy_texture(SDL_Texture*& texture) {
      co_await fabric::this_task::switch_executor(executor_);
      PROF_SCOPE(SDL3::destroy_texture);
      SDL_DestroyTexture(texture);
      texture = nullptr;
    }
  };
} // namespace cydui::platform::window
