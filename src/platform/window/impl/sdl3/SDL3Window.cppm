/*! \file  SDL3Window.cppm
 *! \brief
 *!
 */

module;
#include <tracy/Tracy.hpp>
#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>

export module cydui.plaform.window.sdl3.window;

import std;
import reflect;

export import cydui.platform.window.window_base;

namespace cydui::platform::window {
  export class SDL3Window final: public WindowBase {
    SDL_Window*   window_       = nullptr;
    SDL_Renderer* sdl_renderer_ = nullptr;
    SDL_Texture*  texture_      = nullptr;

  public:
    using sptr = std::shared_ptr<SDL3Window>;

    SDL3Window(
      const std::string& title,
      int                width,
      int                height
    ) {
      // get_executor()->schedule([title] -> fabric::task<> {
      //   tracy::SetThreadNameWithHint(std::format("window[{}]", title).c_str(), 1);
      //   co_return;
      // });
      if (not SDL_CreateWindowAndRenderer(
            title.c_str(),
            width,
            height,
            SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL,
            &window_,
            &sdl_renderer_
          )) {
        SDL_Log("Couldn't create Window: %s", SDL_GetError());
        LOG::print{ERROR}("Couldn't create Window: {}", SDL_GetError());
      }

      // SDL_FlashWindow(win_ref->window, SDL_FLASH_UNTIL_FOCUSED);
    }

    SDL3Window(const SDL3Window&)            = delete;
    SDL3Window& operator=(const SDL3Window&) = delete;
    SDL3Window(SDL3Window&&)                 = delete;
    SDL3Window& operator=(SDL3Window&&)      = delete;

    ~SDL3Window() override {
      SDL_DestroyWindow(window_);
      SDL_DestroyRenderer(sdl_renderer_);
    }

    id_type get_id() const override {
      return SDL_GetWindowID(window_);
    }

    void set_position(
      int x,
      int y
    ) override {
      SDL_SetWindowPosition(window_, x, y);
    }

    void enable_text_input() override {
      SDL_StartTextInput(window_);
    }

    void disable_text_input() override {
      SDL_StopTextInput(window_);
    }

    std::pair<
      int,
      int>
    get_size() override {
      int w, h;
      SDL_GetCurrentRenderOutputSize(sdl_renderer_, &w, &h);
      // SDL_GetWindowSize(window_, &w, &h);
      return {w, h};
    }

    void present(const Surface& surface) override {
      SDL_Texture* texture = static_cast<SDL_Texture*>(surface.handle);
      SDL_FRect    src{
           .x = 0,
           .y = 0,
           .w = static_cast<float>(surface.size.first),
           .h = static_cast<float>(surface.size.second)
      };
      // SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND_PREMULTIPLIED);

      // SDL_SetRenderDrawColor(renderer_, 0xff, 0, 0xff, 0xff);
      // SDL_RenderFillRect(renderer_, &src);
      SDL_RenderTexture(sdl_renderer_, texture, &src, &src);
      // SDL_FRect r{.x = 100, .y = 210, .w = 100, .h = 100};
      // SDL_SetRenderDrawColor(renderer_, 0, 0xff, 0xff, 0xff);
      // SDL_RenderFillRect(renderer_, &r);

      // SDL_Surface* thumbnail = read_texture_pixels(texture_ptr, 4);
      // if (nullptr != thumbnail) {
      //   FrameImage(thumbnail->pixels, thumbnail->w, thumbnail->h, 0, false);
      //   SDL_DestroySurface(thumbnail);
      // }
      SDL_RenderPresent(sdl_renderer_);
    }

    fabric::task<> event_task() override {
      // TODO - Implement
      co_return;
    }
  };
} // namespace cydui::platform::window
