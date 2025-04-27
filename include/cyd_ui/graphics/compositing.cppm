// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include <cairomm-1.16/cairomm/cairomm.h>
#include <tracy/Tracy.hpp>

#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>

export module cydui.graphics:compositing;

import std;

import fabric.profiling;

import :api;
export import cydui.graphics.vector;

import cydui.application;
import cydui.graphics.window;

export import :dev_texture;

export namespace cydui {
  enum class overflow_e { GROW, HIDE, SCROLL };
}

export namespace cydui::compositing {
  using namespace vg;
  using namespace std::chrono_literals;

  struct compositing_operation_t {
    enum {
      OVERLAY,
    } op = OVERLAY;

    int    x, y, orig_x, orig_y;
    int    w, h;
    double rot      = 0.0;
    double scale_x  = 1.0;
    double scale_y  = 1.0;
    bool   animated = false;

    overflow_e x_overflow = overflow_e::HIDE;
    overflow_e y_overflow = overflow_e::HIDE;

    std::function<void(compositing_operation_t& op)> _fix_dimensions =
      [](compositing_operation_t& op) {};

    bool operator==(const compositing_operation_t& other) const {
      return x == other.x && y == other.y && orig_x == other.orig_x && orig_y == other.orig_y
             && w == other.w && h == other.h && scale_x == other.scale_x && scale_y == other.scale_y
             && rot == other.rot && animated == other.animated;
    }
  };

  struct compositing_node_t {
    bool is_out_of_bounds() {
      if (nullptr != parent) {
        auto& pop             = parent->op;
        bool  x_out_of_bounds = pop.orig_x + op.x > pop.w or pop.orig_x + op.x < -op.w;
        bool  y_out_of_bounds = pop.orig_y + op.y > pop.h or pop.orig_y + op.y < -op.h;
        if (pop.x_overflow == overflow_e::GROW) {
          x_out_of_bounds = false;
        }
        if (pop.y_overflow == overflow_e::GROW) {
          y_out_of_bounds = false;
        }
        return x_out_of_bounds or y_out_of_bounds;
      }
      return false;
    }
    compositing_node_t* get_parent() {
      return parent;
    }
    void set_parent(compositing_node_t* parent_) {
      parent = parent_;
      is_flattened =
        (not op.animated) and ((op.op == compositing_operation_t::OVERLAY) and (parent != nullptr));
      // and (op.x >= 0) && (op.y >= 0)
      // and ((op.x + op.w) <= parent->op.w)
      // and ((op.y + op.h) <= parent->op.h));

      if (is_flattened) {
        auto rel_x        = parent->op.orig_x + op.x;
        auto rel_y        = parent->op.orig_y + op.y;
        flattening_target = parent->flattening_target;
        flatten_x         = parent->flatten_x + rel_x;
        flatten_y         = parent->flatten_y + rel_y;
      } else {
        flattening_target = this;
        flatten_x         = 0;
        flatten_y         = 0;
      }
      if (nullptr == parent) {
        flatten_vbox_x = 0;
        flatten_vbox_y = 0;
        flatten_vbox_w = op.w;
        flatten_vbox_h = op.h;
      } else {
        auto rel_x     = parent->op.orig_x - parent->flatten_vbox_x + op.x;
        auto rel_y     = parent->op.orig_y - parent->flatten_vbox_y + op.y;
        flatten_vbox_x = std::max(0, -rel_x);
        flatten_vbox_y = std::max(0, -rel_y);
        if (rel_x < 0) {
          flatten_vbox_w = std::min(rel_x + op.w, parent->flatten_vbox_w);
        } else {
          flatten_vbox_w = std::min(op.w, parent->flatten_vbox_w - rel_x);
        }
        if (rel_y < 0) {
          flatten_vbox_h = std::min(rel_y + op.h, parent->flatten_vbox_h);
        } else {
          flatten_vbox_h = std::min(op.h, parent->flatten_vbox_h - rel_y);
        }

        if (parent->op.x_overflow == overflow_e::GROW) {
          flatten_vbox_x = 0;
          flatten_vbox_w = op.w;
        }
        if (parent->op.y_overflow == overflow_e::GROW) {
          flatten_vbox_y = 0;
          flatten_vbox_h = op.h;
        }
      }
    }

    bool is_flattened_node() const {
      return is_flattened;
    }

    void mark_flattening_target_dirty() {
      flattening_target->flattening_dirty = true;
    }

    bool is_dirty_from_flattening() const {
      return flattening_target->flattening_dirty;
    }

    void start_render(graphics::window_t* render_target) {
      if (is_flattened) {
        pixel_stride = flattening_target->pixel_stride;
        pixels       = &parent->pixels
                    [(parent->op.orig_x + op.x) + (pixel_stride >> 2) * (parent->op.orig_y + op.y)];
      } else {
        ZoneScopedN("Start render");

        if (rendered_texture.is_locked()) {
          rendered_texture.unlock();
        }
        if (op.w == 0 or op.h == 0) {
          rendered_texture.resize(render_target->renderer, 1, 1);
        } else if (op.w != rendered_texture.width() || op.h != rendered_texture.height()) {
          rendered_texture.resize(render_target->renderer, op.w, op.h);
        }

        pixel_stride = rendered_texture.width() * sizeof(pixel_t);
        pixels       = static_cast<pixel_t*>(rendered_texture.lock());
      }
    }

    void render(graphics::window_t* render_target) {
      if (op.w == 0 or op.h == 0) {
        return;
      }
      ZoneScopedN("Render Node");

      pixelmap_editor_t editor{op.w, op.h, pixels, pixel_stride};

      if (not is_flattened) {
        editor.clear();
      }
      editor->begin_new_path();
      editor->rectangle(flatten_vbox_x, flatten_vbox_y, flatten_vbox_w, flatten_vbox_h);
      editor->clip();

      if (not graphics.empty()) {
        for (const auto& element: graphics.elements) {
          element->_internal_set_origin(op.orig_x, op.orig_y);
          element->apply_to(editor);
        }
      }

      flattening_target->dirty_ = true;
    }

    void flush_rendered_texture(graphics::window_t* render_target) {
      if (dirty_) {
        ZoneScopedN("Flush Rendered Texture");
        rendered_texture.unlock();
        composite_texture.resize(
          render_target->renderer, rendered_texture.width(), rendered_texture.height()
        );
        dirty_ = false;
      }
    }

    void clear_composite_texture(graphics::window_t* render_target) {
      ZoneScopedN("Clear Composite Texture");
      if (not is_flattened) {
        composite_texture.clear(render_target->renderer);
      }
    }

    void compose_own(graphics::window_t* render_target) {
      ZoneScopedN("Compose Own");
      if (is_flattened)
        return;
      flush_rendered_texture(render_target);
      SDL_FRect dst{
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

      auto& target = is_flattened ? flattening_target->composite_texture : composite_texture;

      float w_ = other->composite_texture.width();
      float h_ = other->composite_texture.height();

      SDL_FRect src{
        .x = static_cast<float>(other->flatten_vbox_x),
        .y = static_cast<float>(other->flatten_vbox_y),
        .w = static_cast<float>(other->flatten_vbox_w),
        .h = static_cast<float>(other->flatten_vbox_h),
      };
      SDL_FRect dst{
        .x = static_cast<float>(flatten_x + op.orig_x + other->op.x),
        .y = static_cast<float>(flatten_y + op.orig_y + other->op.y),
        .w = static_cast<float>(other->flatten_vbox_w),
        .h = static_cast<float>(other->flatten_vbox_h),
      };

      float new_w = target.width();
      float new_h = target.height();

      if (op.x_overflow == overflow_e::GROW) {
        new_w = std::max(target.width(), flatten_x + op.orig_x + other->op.x + w_);
      } else {
        if (dst.x > target.width() or dst.x < -w_) {
          return;
        }
        dst.x += flatten_vbox_x;
      }

      if (op.y_overflow == overflow_e::GROW) {
        new_h = std::max(target.height(), flatten_y + op.orig_y + other->op.y + h_);
      } else {
        if (dst.y > target.height() or dst.y < -w_) {
          return;
        }
        dst.y += flatten_vbox_y;
      }

      target.resize(renderer, new_w, new_h, true);

      other->composite_texture.copy_into(renderer, target, &dst, true, &src);
    }

  private:
    bool dirty_ = true;

    compositing_node_t* parent            = nullptr;
    compositing_node_t* flattening_target = nullptr;
    bool                flattening_dirty  = false;
    bool                is_flattened      = false;
    int                 pixel_stride      = 0;

    int flatten_x = 0;
    int flatten_y = 0;

    int flatten_vbox_x = 0;
    int flatten_vbox_y = 0;
    int flatten_vbox_w = 0;
    int flatten_vbox_h = 0;

  public:
    unsigned long           id = 0;
    compositing_operation_t op{};
    fragment_t              graphics{};

    device_texture_t rendered_texture{true};
    device_texture_t composite_texture{};
    pixel_t*         pixels;
  };

  class LayoutCompositor {
    graphics::window_t* render_target = nullptr;
    prof::context_t*    profiler      = nullptr;

  public:
    LayoutCompositor() {}

    ~LayoutCompositor() {}

    void set_render_target(graphics::window_t* _render_target, prof::context_t* _profiler) {
      render_target = _render_target;
      profiler      = _profiler;
    }

    void compose(compositing_node_t* root) {
      auto* rtarget = render_target;
      if (root != nullptr) {
        SDL_SetRenderDrawColor(rtarget->renderer, 0, 0, 0, 0);
        SDL_RenderClear(rtarget->renderer);

        SDL_Texture* texture = root->composite_texture.sdl_texture();
        SDL_FRect    src{
             .x = 0,
             .y = 0,
             .w = root->composite_texture.width(),
             .h = root->composite_texture.height()
        };
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND_PREMULTIPLIED);

        SDL_RenderTexture(rtarget->renderer, texture, &src, &src);
        SDL_RenderPresent(rtarget->renderer);
      }
    }
  };
} // namespace cydui::compositing
