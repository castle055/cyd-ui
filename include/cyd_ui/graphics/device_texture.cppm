// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include <cairomm-1.16/cairomm/cairomm.h>
#include <tracy/Tracy.hpp>

#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>

export module cydui.graphics:dev_texture;

import std;

import fabric.profiling;

import cydui.graphics.types;
import cydui.application;

export namespace cydui::compositing {
  struct device_texture_t {
    using texture_ptr = SDL_Texture*;

    device_texture_t(bool streaming = false): w(0), h(0), streaming_(streaming) {
    }
    device_texture_t(const device_texture_t &other): renderer_(other.renderer_), w(other.w), h(other.h), streaming_(other.streaming_) {
      if (other.texture != nullptr) {
        texture = SDL_CreateTexture(renderer_, SDL_PIXELFORMAT_BGRA32, streaming_? SDL_TEXTUREACCESS_STREAMING: SDL_TEXTUREACCESS_TARGET, w, h);
        other.copy_into(renderer_, *this, nullptr);
      }
    }
    device_texture_t& operator=(const device_texture_t &other) {
      this->renderer_ = other.renderer_;
      this->w = other.w;
      this->h = other.h;
      this->streaming_ = other.streaming_;
      if (other.texture != nullptr) {
        texture = SDL_CreateTexture(renderer_, SDL_PIXELFORMAT_BGRA32, streaming_? SDL_TEXTUREACCESS_STREAMING: SDL_TEXTUREACCESS_TARGET, w, h);
        other.copy_into(renderer_, *this, nullptr);
      }
      return *this;
    }
    device_texture_t(device_texture_t &&other) noexcept: renderer_(other.renderer_), w(other.w), h(other.h), streaming_(other.streaming_) {
      if (other.texture != nullptr) {
        texture = other.texture;
        other.texture = nullptr;
      }
    }
    device_texture_t& operator=(device_texture_t &&other) noexcept {
      this->renderer_ = other.renderer_;
      this->w = other.w;
      this->h = other.h;
      this->streaming_ = other.streaming_;
      if (other.texture != nullptr) {
        texture = other.texture;
        other.texture = nullptr;
      }
      return *this;
    }
    ~device_texture_t() {
      SDL_DestroyTexture(texture);
    }

    void resize(SDL_Renderer* renderer, float w, float h, bool copy_old = false) {
      renderer_ = renderer;
      if (w != this->w || h != this->h) {
        ZoneScopedN("Resize Texture");
        SDL_Texture* old_texture = texture;
        texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_BGRA32, streaming_? SDL_TEXTUREACCESS_STREAMING: SDL_TEXTUREACCESS_TARGET, w, h);

        if (old_texture != nullptr) {
          if (copy_old) {
            SDL_SetRenderTarget(renderer, texture);
            SDL_SetTextureBlendMode(old_texture, SDL_BLENDMODE_BLEND);
            SDL_FRect dst {
              .x = 0,
              .y = 0,
              .w = std::min(w, this->w),
              .h = std::min(h, this->h)
            };
            SDL_RenderTexture(renderer, old_texture, &dst, &dst);
            SDL_SetRenderTarget(renderer, nullptr);
          }
          SDL_DestroyTexture(old_texture);
        }

        this->w = w;
        this->h = h;
      }
    }

    void clear(SDL_Renderer* renderer) {
      renderer_ = renderer;
      if (texture == nullptr) return;
      ZoneScopedN("Clear Texture");
      SDL_SetRenderTarget(renderer, texture);
      SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
      SDL_RenderClear(renderer);
      SDL_SetRenderTarget(renderer, nullptr);
    }

    void update_with(const pixelmap_t& pm) {
      if (texture == nullptr) return;
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

    void* lock() {
      if (texture == nullptr) return nullptr;
      ZoneScopedN("Lock Texture");
      void* pixels = nullptr;
      int pitch;
      locked_ = true;
      SDL_LockTexture(texture, nullptr, &pixels, &pitch);
      return pixels;
    }

    void unlock() {
      if (texture == nullptr) return;
      ZoneScopedN("Unlock Texture");
      SDL_UnlockTexture(texture);
      locked_ = false;
    }

    bool is_locked() const {
      return locked_;
    }

    void copy_into(SDL_Renderer* renderer, device_texture_t& other, SDL_FRect* dst, bool blend = true, SDL_FRect* src_ = nullptr) {
      renderer_ = renderer;
      if (texture == nullptr) return;
      ZoneScopedN("Copying Texture");
      SDL_SetRenderTarget(renderer, other.texture);
      if (blend) {
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
      } else {
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_NONE);
      }
      SDL_FRect src {
        .x = 0,
        .y = 0,
        .w = this->w,
        .h = this->h
      };
      if (src_ != nullptr) {
        src = *src_;
      }

      SDL_RenderTexture(renderer, texture, &src, dst);
      SDL_SetRenderTarget(renderer, nullptr);
    }

    void copy_into(SDL_Renderer* renderer, device_texture_t& other, SDL_FRect* dst) const {
      if (texture == nullptr) return;
      ZoneScopedN("Copying Texture");
      SDL_SetRenderTarget(renderer, other.texture);
      SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
      SDL_FRect src {
        .x = 0,
        .y = 0,
        .w = this->w,
        .h = this->h
      };
      SDL_RenderTexture(renderer, texture, &src, dst);
      SDL_SetRenderTarget(renderer, nullptr);
    }

    float width() const {
      return w;
    }
    float height() const {
      return h;
    }

    SDL_Texture* sdl_texture() {
      return texture;
    }

  private:
    SDL_Renderer* renderer_ = nullptr;
    texture_ptr texture {nullptr};
    float w, h;
    bool streaming_ = false;
    bool locked_ = false;
  };
}
