/*! \file  frame.cppm
 *! \brief
 *!
 */

module;
#include <tracy/Tracy.hpp>
#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>

export module cydui.backends.sdl3:frame;
import :frame_map;
import :renderer;

import std;
export import reflect;

export import cydui.backends.frame_base;

namespace cydui::backends {
  export class SDL3_frame: public frame_base {
    frame_map::sptr     registered_frame_map_;
    SDL_Window*         window_       = nullptr;
    SDL_Renderer*       sdl_renderer_ = nullptr;
    SDL_Texture*        texture_      = nullptr;
    SDL3_renderer::sptr renderer_     = nullptr;

  public:
    using sptr = std::shared_ptr<SDL3_frame>;

    SDL3_frame(const frame_map::sptr& f_map, const std::string& title, int width, int height)
        : registered_frame_map_(f_map) {
      get_executor()->schedule([title] -> fabric::task<> {
        tracy::SetThreadNameWithHint(std::format("window[{}]", title).c_str(), 1);
        co_return;
      });
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
      renderer_ = std::make_shared<SDL3_renderer>(sdl_renderer_);

      // SDL_FlashWindow(win_ref->window, SDL_FLASH_UNTIL_FOCUSED);
    }

    SDL3_frame(const SDL3_frame&)            = delete;
    SDL3_frame& operator=(const SDL3_frame&) = delete;
    SDL3_frame(SDL3_frame&&)                 = delete;
    SDL3_frame& operator=(SDL3_frame&&)      = delete;

    ~SDL3_frame() override {
      registered_frame_map_->unregister_frame(SDL3_frame::get_id());
      SDL_DestroyWindow(window_);
      SDL_DestroyRenderer(sdl_renderer_);
    }

    id_type get_id() const override {
      return SDL_GetWindowID(window_);
    }

    renderer_base::sptr get_renderer() override {
      return renderer_;
    }

    void set_position(int x, int y) override {
      SDL_SetWindowPosition(window_, x, y);
    }

    void enable_text_input() override {
      SDL_StartTextInput(window_);
    }

    void disable_text_input() override {
      SDL_StopTextInput(window_);
    }

    std::pair<int, int> get_size() override {
      int w, h;
      SDL_GetCurrentRenderOutputSize(sdl_renderer_, &w, &h);
      // SDL_GetWindowSize(window_, &w, &h);
      return {w, h};
    }

    texture_base::sptr make_texture(bool is_target) override {
      return std::make_shared<device_texture_t>(not is_target);
    }
  };


} // namespace cydui::backends
