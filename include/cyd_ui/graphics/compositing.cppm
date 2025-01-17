// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include <cairomm-1.16/cairomm/cairomm.h>
#include <tracy/Tracy.hpp>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

export module cydui.graphics:compositing;

import std;

import fabric.profiling;

import :api;
import :vg;

import cydui.application;
import cydui.graphics.window;

export import :dev_texture;

export namespace cyd::ui::compositing {
  using namespace vg;
  using namespace std::chrono_literals;

  struct compositing_operation_t {
    enum {
      OVERLAY,
    } op = OVERLAY;

    int x, y, orig_x, orig_y;
    int w, h;
    double rot = 0.0;
    double scale_x = 1.0;
    double scale_y = 1.0;

    std::function<void(compositing_operation_t &op)> _fix_dimensions = [](compositing_operation_t &op) {
    };
  };

  struct compositing_node_t {
    void start_render(graphics::window_t* render_target) {
      if (op.w == 0 or op.h == 0) {
        rendered_texture.resize(render_target->renderer, 0, 0);
        return;
      }
      ZoneScopedN("Start render");

      if (op.w != rendered_texture.width() || op.h != rendered_texture.height()) {
        rendered_texture.resize(render_target->renderer, op.w, op.h);
      }

      pixels = static_cast<pixel_t*>(rendered_texture.lock());
    }

    void render(graphics::window_t* render_target) {
      if (op.w == 0 or op.h == 0) {
        return;
      }
      ZoneScopedN("Render Node");

      pixelmap_editor_t editor {op.w, op.h, pixels};
      editor.clear();

      if (not graphics.empty()) {
        for (const auto &element: graphics.elements) {
          element->_internal_set_origin(
            op.orig_x,
            op.orig_y
          );
          element->apply_to(editor);
        }
      }

      dirty_ = true;
    }

    void flush_rendered_texture(graphics::window_t* render_target) {
      if (dirty_) {
        ZoneScopedN("Flush Rendered Texture");
        rendered_texture.unlock();
        composite_texture.resize(render_target->renderer, rendered_texture.width(), rendered_texture.height());
        dirty_ = false;
      }
    }

    void clear_composite_texture(graphics::window_t* render_target) {
      ZoneScopedN("Clear Composite Texture");
      composite_texture.clear(render_target->renderer);
    }

    void compose_own(graphics::window_t* render_target) {
      ZoneScopedN("Compose Own");
      flush_rendered_texture(render_target);
      SDL_Rect dst {
        .x = 0,
        .y = 0,
        .w = rendered_texture.width(),
        .h = rendered_texture.height(),
      };
      rendered_texture.copy_into(render_target->renderer, composite_texture, &dst, false);
    }

    void compose(graphics::window_t* render_target, compositing_node_t* other) {
      ZoneScopedN("Compose Child");
      SDL_Renderer* renderer = render_target->renderer;

      composite_texture.resize(
        renderer,
        std::max(composite_texture.width(), other->op.x + other->composite_texture.width()),
        std::max(composite_texture.height(), other->op.y + other->composite_texture.height())
      );

      SDL_Rect dst {
        .x = other->op.x,
        .y = other->op.y,
        .w = other->composite_texture.width(),
        .h = other->composite_texture.height(),
      };
      other->composite_texture.copy_into(renderer, this->composite_texture, &dst);
    }

  private:
    bool dirty_ = true;
  public:
    unsigned long id = 0;
    compositing_operation_t op { };
    vg_fragment_t graphics { };

    device_texture_t rendered_texture {true};
    device_texture_t composite_texture { };
    pixel_t* pixels;
  };

  class LayoutCompositor {
    graphics::window_t* render_target = nullptr;
    prof::context_t* profiler = nullptr;

  public:
    LayoutCompositor() {
    }

    ~LayoutCompositor() {
    }

    void set_render_target(graphics::window_t* _render_target, prof::context_t* _profiler) {
      render_target = _render_target;
      profiler = _profiler;
    }

    void compose(compositing_node_t* root) {
      auto* rtarget = render_target;
      if (root != nullptr) {
        SDL_SetRenderTarget(rtarget->renderer, nullptr);

        SDL_SetRenderDrawColor(rtarget->renderer, 0, 0, 0, 0);
        SDL_RenderClear(rtarget->renderer);

        SDL_Texture* texture = root->composite_texture.sdl_texture();
        SDL_Rect src {
          .x = 0,
          .y = 0,
          .w = root->composite_texture.width(),
          .h = root->composite_texture.height()
        };
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
        SDL_RenderCopy(rtarget->renderer, texture, &src, &src);
        SDL_RenderPresent(rtarget->renderer);
      }
    }
  };
}
