// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include <cairomm-1.16/cairomm/cairomm.h>
#include <tracy/Tracy.hpp>

#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>

#define ASSERT_OK(...)                                                                             \
  if (not(__VA_ARGS__)) {                                                                          \
    throw std::runtime_error(SDL_GetError());                                                      \
  }

export module cydui.backends.sdl3:dev_texture;

import std;

import fabric.profiling;

import cydui.graphics.types;
import cydui.application;
export import cydui.backends.texture_base;

export namespace cydui::backends {
  struct device_texture_t: public backends::texture_base {
    using sptr        = std::shared_ptr<device_texture_t>;
    using texture_ptr = SDL_Texture*;

    device_texture_t(bool streaming = false)
        : w(0),
          h(0),
          streaming_(streaming) {}

    device_texture_t(const device_texture_t& other)
        : renderer_(other.renderer_),
          w(other.w),
          h(other.h),
          streaming_(other.streaming_) {
      if (other.texture != nullptr) {
        texture = SDL_CreateTexture(
          renderer_,
          streaming_ ? SDL_PIXELFORMAT_BGRA32 : SDL_PIXELFORMAT_RGBA8888,
          SDL_TEXTUREACCESS_TARGET,
          w,
          h
        );
        other.copy_into(renderer_, *this, nullptr);
      }
    }
    device_texture_t& operator=(const device_texture_t& other) {
      this->renderer_  = other.renderer_;
      this->w          = other.w;
      this->h          = other.h;
      this->streaming_ = other.streaming_;
      if (other.texture != nullptr) {
        texture = SDL_CreateTexture(
          renderer_,
          streaming_ ? SDL_PIXELFORMAT_BGRA32 : SDL_PIXELFORMAT_RGBA8888,
          SDL_TEXTUREACCESS_TARGET,
          w,
          h
        );
        other.copy_into(renderer_, *this, nullptr);
      }
      return *this;
    }
    device_texture_t(device_texture_t&& other) noexcept
        : renderer_(other.renderer_),
          w(other.w),
          h(other.h),
          streaming_(other.streaming_) {
      if (other.texture != nullptr) {
        texture       = other.texture;
        other.texture = nullptr;
      }
    }
    device_texture_t& operator=(device_texture_t&& other) noexcept {
      this->renderer_  = other.renderer_;
      this->w          = other.w;
      this->h          = other.h;
      this->streaming_ = other.streaming_;
      if (other.texture != nullptr) {
        texture       = other.texture;
        other.texture = nullptr;
      }
      return *this;
    }
    ~device_texture_t() {
      SDL_DestroyTexture(texture);
    }

    void resize(
      SDL_Renderer* renderer,
      float         w,
      float         h,
      bool          copy_old = false
    ) {
      renderer_ = renderer;
      if (w <= 0 or h <= 0 or (w == this->w and h == this->h)) {
        return;
      }

      if (w > this->dev_w || h > this->dev_h //
          || w < this->dev_w * 0.75f || h < this->dev_h * 0.75f) {
        ZoneScopedN("Resize Texture");
        SDL_Texture* old_texture = texture;
        texture                  = SDL_CreateTexture(
          renderer,
          streaming_ ? SDL_PIXELFORMAT_BGRA32 : SDL_PIXELFORMAT_RGBA8888,
          SDL_TEXTUREACCESS_TARGET,
          w,
          h
        );
        ASSERT_OK(nullptr != texture);

        if (old_texture != nullptr) {
          if (copy_old) {
            SDL_SetRenderTarget(renderer, texture);
            SDL_SetTextureBlendMode(old_texture, SDL_BLENDMODE_BLEND);
            SDL_FRect dst{.x = 0, .y = 0, .w = std::min(w, this->w), .h = std::min(h, this->h)};
            SDL_RenderTexture(renderer, old_texture, &dst, &dst);
            SDL_SetRenderTarget(renderer, nullptr);
          }
          SDL_DestroyTexture(old_texture);
        }
        this->dev_w = w;
        this->dev_h = h;
      }

      this->w = w;
      this->h = h;

      pitch_ = this->dev_w * 4;
      pixels_.resize(this->dev_w * this->dev_h * 4);
    }

    void clear(SDL_Renderer* renderer) {
      renderer_ = renderer;
      if (texture == nullptr)
        return;
      ZoneScopedN("Clear Texture");
      SDL_SetRenderTarget(renderer, texture);
      SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
      SDL_RenderClear(renderer);
      SDL_SetRenderTarget(renderer, nullptr);
    }

    void update_with(const pixelmap_t& pm) {
      if (texture == nullptr)
        return;
      ZoneScopedN("Update Texture");
      // if (streaming_) {
      //   void* pixels = nullptr;
      //   int pitch;
      //   SDL_LockTexture(texture, nullptr, &pixels, &pitch);
      //   std::memcpy(pixels, pm.data, pm.height() * pm.width() * sizeof(pixel_t));
      //   SDL_UnlockTexture(texture);
      // } else {
      SDL_UpdateTexture(texture, nullptr, pm.data, pm.width() * sizeof(pixel_t));
      // }
    }

    std::pair<
      void*,
      int>
    lock() {
      if (texture == nullptr)
        return {nullptr, 0};
      ZoneScopedN("Lock Texture");
      // ASSERT_OK(SDL_LockTexture(texture, nullptr, &pixels_, &pitch_));
      locked_ = true;
      return {pixels_.data(), pitch_};
    }

    void unlock() {
      if (texture == nullptr)
        return;
      ZoneScopedN("Unlock Texture");
      // SDL_UnlockTexture(texture);
      SDL_Rect rect{
        .x = 0, .y = 0, .w = static_cast<int>(width()), .h = static_cast<int>(height())
      };
      ASSERT_OK(SDL_UpdateTexture(texture, &rect, pixels_.data(), pitch_));
      locked_ = false;
    }

    bool is_locked() const {
      return locked_;
    }

    void copy_into(
      SDL_Renderer*     renderer,
      device_texture_t& other,
      SDL_FRect*        dst,
      bool              blend = true,
      SDL_FRect* src_         = nullptr
    ) {
      renderer_ = renderer;
      if (texture == nullptr)
        return;
      ZoneScopedN("Copying Texture");
      ASSERT_OK(SDL_SetRenderTarget(renderer, other.texture));

      if (blend) {
        ASSERT_OK(SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND));
      } else {
        ASSERT_OK(SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_NONE));
      }
      SDL_FRect src{.x = 0, .y = 0, .w = this->w, .h = this->h};
      if (src_ != nullptr) {
        src = *src_;
      }

      ASSERT_OK(SDL_RenderTexture(renderer, texture, &src, dst));
      ASSERT_OK(SDL_SetRenderTarget(renderer, nullptr));
    }

    void copy_into(
      SDL_Renderer*     renderer,
      device_texture_t& other,
      SDL_FRect*        dst
    ) const {
      if (texture == nullptr)
        return;
      ZoneScopedN("Copying Texture");
      ASSERT_OK(SDL_SetRenderTarget(renderer, other.texture));
      ASSERT_OK(SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND));
      SDL_FRect src{.x = 0, .y = 0, .w = this->w, .h = this->h};
      ASSERT_OK(SDL_RenderTexture(renderer, texture, &src, dst));
      ASSERT_OK(SDL_SetRenderTarget(renderer, nullptr));
    }

    float width() const override {
      return w;
    }
    float height() const override {
      return h;
    }

    SDL_Texture* sdl_texture() {
      return texture;
    }

    std::pair<
      void*,
      int>
    begin_render() override {
      if (not is_locked()) {
        return lock();
      }
      return {pixels_.data(), pitch_};
    }

    void end_render() override {
      if (is_locked()) {
        unlock();
      }
    }

  private:
    [[refl::ignore]]
    SDL_Renderer*        renderer_ = nullptr;
    texture_ptr          texture{nullptr};
    float                w, h;
    float                dev_w, dev_h;
    bool                 streaming_ = false;
    bool                 locked_    = false;
    std::vector<pixel_t> pixels_{};
    int                  pitch_ = 0;
  };
} // namespace cydui::backends
