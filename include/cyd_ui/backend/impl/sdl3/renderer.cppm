/*! \file  renderer.cppm
 *! \brief
 *!
 */

module;
#include "cyd_ui/debug/profiling/macros.h"
#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>

export module cydui.backends.sdl3:renderer;
import :dev_texture;

import std;
export import reflect;
import cydui.debug.profiling;

export import cydui.backends.renderer_base;
export import cydui.graphics.compositing_node;

namespace cydui::backends {
  export class SDL3_renderer: public renderer_base {
    SDL_Renderer* renderer_ = nullptr;

  public:
    using sptr = std::shared_ptr<SDL3_renderer>;

    explicit SDL3_renderer(SDL_Renderer* renderer)
        : renderer_(renderer) {}

    void clear() override {
      SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 0);
      SDL_RenderClear(renderer_);
    }

    void render_texture(const texture_base::sptr& texture_) override {
      auto         texture_ptr = std::dynamic_pointer_cast<device_texture_t>(texture_);
      SDL_Texture* texture     = texture_ptr->sdl_texture();
      SDL_FRect    src{.x = 0, .y = 0, .w = texture_ptr->width(), .h = texture_ptr->height()};
      SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND_PREMULTIPLIED);

      // SDL_SetRenderDrawColor(renderer_, 0xff, 0, 0xff, 0xff);
      // SDL_RenderFillRect(renderer_, &src);
      SDL_RenderTexture(renderer_, texture, &src, &src);
      // SDL_FRect r{.x = 100, .y = 210, .w = 100, .h = 100};
      // SDL_SetRenderDrawColor(renderer_, 0, 0xff, 0xff, 0xff);
      // SDL_RenderFillRect(renderer_, &r);

      // SDL_Surface* thumbnail = read_texture_pixels(texture_ptr, 4);
      // if (nullptr != thumbnail) {
      //   FrameImage(thumbnail->pixels, thumbnail->w, thumbnail->h, 0, false);
      //   SDL_DestroySurface(thumbnail);
      // }
    }

    void present() override {
      SDL_RenderPresent(renderer_);
    }

    void resize_texture(
      const texture_base::sptr& texture,
      float                     new_width,
      float                     new_heigth,
      bool                      copy_old
    ) override {
      auto texture_ptr = std::dynamic_pointer_cast<device_texture_t>(texture);
      texture_ptr->resize(renderer_, new_width, new_heigth, copy_old);
    }

    void copy_texture(
      const texture_base::sptr& source,
      const texture_base::sptr& destination,
      rect*                     source_rect,
      rect*                     dest_rect,
      bool                      blend
    ) override {
      auto source_ptr = std::dynamic_pointer_cast<device_texture_t>(source);
      auto dest_ptr   = std::dynamic_pointer_cast<device_texture_t>(destination);

      source_ptr->copy_into(
        renderer_,
        *dest_ptr,
        reinterpret_cast<SDL_FRect*>(dest_rect),
        blend,
        reinterpret_cast<SDL_FRect*>(source_rect)
      );
    }

  private:
    SDL_Surface* read_texture_pixels(
      const device_texture_t::sptr& texture,
      int                           scaleDivider
    ) {
      int dstW = texture->width() / scaleDivider;
      int dstH = texture->height() / scaleDivider;

      dstW -= dstW % 4U;
      dstH -= dstH % 4U;

      if (dstW <= 0 || dstH <= 0) {
        return nullptr;
      }

      device_texture_t target{};
      target.resize(renderer_, dstW, dstH, false);
      SDL_SetRenderTarget(renderer_, target.sdl_texture());

      SDL_FRect srcFRect = {0, 0, texture->width(), texture->height()};
      SDL_FRect dstFRect = {0, 0, 0.0f + dstW, 0.0f + dstH};
      SDL_Rect  dstRect  = {0, 0, dstW, dstH};

      SDL_RenderTexture(renderer_, texture->sdl_texture(), &srcFRect, &dstFRect);

      SDL_Surface* surface = SDL_RenderReadPixels(renderer_, &dstRect);
      SDL_SetRenderTarget(renderer_, nullptr);

      SDL_Surface* correct_surface = SDL_ConvertSurface(surface, SDL_PIXELFORMAT_RGBA32);
      SDL_DestroySurface(surface);

      return correct_surface;
    }
  };

} // namespace cydui::backends
