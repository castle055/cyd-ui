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

  struct device_texture_t {
    using texture_ptr = SDL_Texture*;

    device_texture_t(): w(0), h(0) {
    }
    device_texture_t(const device_texture_t &other): renderer_(other.renderer_), w(other.w), h(other.h) {
      if (other.texture != nullptr) {
        texture = SDL_CreateTexture(renderer_, SDL_PIXELFORMAT_BGRA32, SDL_TEXTUREACCESS_TARGET, w, h);
        other.copy_into(renderer_, *this, nullptr);
      }
    }
    device_texture_t& operator=(const device_texture_t &other) {
      this->renderer_ = other.renderer_;
      this->w = other.w;
      this->h = other.h;
      if (other.texture != nullptr) {
        texture = SDL_CreateTexture(renderer_, SDL_PIXELFORMAT_BGRA32, SDL_TEXTUREACCESS_TARGET, w, h);
        other.copy_into(renderer_, *this, nullptr);
      }
      return *this;
    }
    device_texture_t(device_texture_t &&other) noexcept: renderer_(other.renderer_), w(other.w), h(other.h) {
      if (other.texture != nullptr) {
        texture = other.texture;
        other.texture = nullptr;
      }
    }
    device_texture_t& operator=(device_texture_t &&other) noexcept {
      this->renderer_ = other.renderer_;
      this->w = other.w;
      this->h = other.h;
      if (other.texture != nullptr) {
        texture = other.texture;
        other.texture = nullptr;
      }
      return *this;
    }
    ~device_texture_t() {
      SDL_DestroyTexture(texture);
    }

    void resize(SDL_Renderer* renderer, int w, int h) {
      renderer_ = renderer;
      if (w != this->w || h != this->h) {
        ZoneScopedN("Resize Texture");
        SDL_Texture* old_texture = texture;
        texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_BGRA32, SDL_TEXTUREACCESS_TARGET, w, h);

        if (texture != nullptr) {
          SDL_SetRenderTarget(renderer, texture);
          SDL_SetTextureBlendMode(old_texture, SDL_BLENDMODE_BLEND);
          SDL_Rect dst {
            .x = 0,
            .y = 0,
            .w = std::min(w, this->w),
            .h = std::min(h, this->h)
          };
          SDL_RenderCopy(renderer, old_texture, &dst, &dst);
          // SDL_SetRenderTarget(renderer, nullptr);
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
      // SDL_SetRenderTarget(renderer, nullptr);
    }

    void update_with(const pixelmap_t& pm) {
      if (texture == nullptr) return;
      ZoneScopedN("Update Texture");
      SDL_UpdateTexture(texture, nullptr, pm.data, pm.width() * 4);
    }

    void copy_into(SDL_Renderer* renderer, device_texture_t& other, SDL_Rect* dst, bool blend = true) {
      renderer_ = renderer;
      if (texture == nullptr) return;
      ZoneScopedN("Copying Texture");
      SDL_SetRenderTarget(renderer, other.texture);
      if (blend) {
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
      } else {
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_NONE);
      }
      SDL_Rect src {
        .x = 0,
        .y = 0,
        .w = this->w,
        .h = this->h
      };
      SDL_RenderCopy(renderer, texture, &src, dst);
      // SDL_RenderFlush(renderer);
      // SDL_SetRenderTarget(renderer, nullptr);
    }

    void copy_into(SDL_Renderer* renderer, device_texture_t& other, SDL_Rect* dst) const {
      if (texture == nullptr) return;
      ZoneScopedN("Copying Texture");
      SDL_SetRenderTarget(renderer, other.texture);
      SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
      SDL_Rect src {
        .x = 0,
        .y = 0,
        .w = this->w,
        .h = this->h
      };
      SDL_RenderCopy(renderer, texture, &src, dst);
      // SDL_RenderFlush(renderer);
      // SDL_SetRenderTarget(renderer, nullptr);
    }

    int width() const {
      return w;
    }
    int height() const {
      return h;
    }

    SDL_Texture* sdl_texture() {
      return texture;
    }

  private:
    SDL_Renderer* renderer_ = nullptr;
    texture_ptr texture {nullptr};
    int w, h;
  };

  struct compositing_node_t {
    // compositing_node_t* find_by_id(unsigned long _id) {
    //   if (id == _id) {
    //     return this;
    //   } else {
    //     for (auto &item: children) {
    //       if (auto* res = item->find_by_id(_id); nullptr != res) {
    //         return res;
    //       }
    //     }
    //     return nullptr;
    //   }
    // }
    //
    void render() {
      if (op.w != rendered_pixelmap->width() || op.h != rendered_pixelmap->height()) {
        rendered_pixelmap->resize({(size_t)op.w, (size_t)op.h});
      }

      pixelmap_editor_t editor {*rendered_pixelmap};
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
        rendered_texture.resize(render_target->renderer, rendered_pixelmap->width(), rendered_pixelmap->height());
        composite_texture.resize(render_target->renderer, rendered_pixelmap->width(), rendered_pixelmap->height());
        rendered_texture.update_with(*rendered_pixelmap);

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
    // std::deque<std::unique_ptr<compositing_node_t>> children { };
    vg_fragment_t graphics { };

    std::shared_ptr<pixelmap_t> rendered_pixelmap {std::make_shared<pixelmap_t>(0, 0)};
    device_texture_t rendered_texture { };
    device_texture_t composite_texture { };

    //void fix_op_dimensions() {
    //  op._fix_dimensions(op);
    //  for (auto &node: children) {
    //    node->fix_op_dimensions();
    //  }
    //}
  };

  struct compositing_tree_t {
    std::unique_ptr<compositing_node_t> root = std::make_unique<compositing_node_t>();

    //void fix_dimensions() {
    //  root.fix_op_dimensions();
    //}
    //
    // compositing_node_t* find_by_id(unsigned long _id) {
    //   return root->find_by_id(_id);
    // }
  };

  class LayoutCompositor {
    graphics::window_t* render_target = nullptr;
    prof::context_t* profiler = nullptr;

    std::shared_ptr<compositing_tree_t> tree = std::make_shared<compositing_tree_t>();
    bool tree_dirty = true;

    std::unordered_map<unsigned long, pixelmap_t*> sub_frame_cache{};

    // pixelmap_t* repaint(
    //   std::unique_ptr<compositing_node_t> &node,
    //   pixelmap_t* frame = nullptr,
    //   std::optional<pixelmap_editor_t*> parent_editor = std::nullopt,
    //   int x_off_in_parent = 0,
    //   int y_off_in_parent = 0
    // ) {
    //   pixelmap_editor_t* editor;
    //   pixelmap_t* frm = frame;
    //   if (!parent_editor.has_value()) {
    //     if (nullptr == frm) {
    //       if (sub_frame_cache.contains(node->id)) {
    //         frm = sub_frame_cache[node->id];
    //         if (frm->width() != (size_t) node->op.w || frm->height() != (size_t) node->op.h) {
    //           frm->resize(
    //             {
    //               (unsigned long) node->op.w,
    //               (unsigned long) node->op.h
    //             }
    //           );
    //         }
    //         editor = new pixelmap_editor_t{frm};
    //         editor->clear();
    //       } else {
    //         frm = new pixelmap_t{
    //           (unsigned long) node->op.w,
    //           (unsigned long) node->op.h
    //         };
    //         sub_frame_cache[node->id] = frm;
    //         editor = new pixelmap_editor_t{frm};
    //       }
    //     } else {
    //       editor = new pixelmap_editor_t{frm};
    //       editor->clear();
    //     }
    //   } else {
    //     editor = parent_editor.value();
    //   }
    //
    //   bool empty = true;
    //   if (!node->graphics.empty()) {
    //     empty = false;
    //     // Rasterize graphics into `frm`
    //     for (const auto &element: node->graphics.elements) {
    //       element->_internal_set_origin(
    //         x_off_in_parent + node->op.orig_x,
    //         y_off_in_parent + node->op.orig_y
    //       );
    //       element->apply_to(*editor);
    //     }
    //   }
    //   if (!node->children.empty()) {
    //     empty = false;
    //     std::vector<std::pair<std::unique_ptr<compositing_node_t> &, pixelmap_t*>> sub_frames{};
    //     for (auto &c: node->children) {
    //       if (c->op.op == compositing_operation_t::OVERLAY) {
    //         repaint(
    //           c,
    //           frm,
    //           editor,
    //           x_off_in_parent + node->op.orig_x + c->op.x,
    //           y_off_in_parent + node->op.orig_y + c->op.y
    //         );
    //       } else {
    //         sub_frames.emplace_back(c, repaint(c));
    //       }
    //     }
    //
    //     // Compose graphics into 'frm'
    //
    //     for (const auto &item: sub_frames) {
    //       auto &[snode, sfrm] = item;
    //       if (nullptr == sfrm) {
    //         continue;
    //       }
    //       auto surface = Cairo::ImageSurface::create(
    //         (unsigned char*) sfrm->data,
    //         Cairo::Surface::Format::ARGB32,
    //         (int) sfrm->width(),
    //         (int) sfrm->height(),
    //         (int) sfrm->width() * 4
    //         //cairo_format_stride_for_width(CAIRO_FORMAT_ARGB32, (int) sfrm->width())
    //       );
    //       (*editor)->save();
    //       // (*editor)->set_antialias(Cairo::ANTIALIAS_NONE);
    //       (*editor)->set_operator(Cairo::Context::Operator::OVER);
    //       (*editor)->set_source(surface, node->op.orig_x + snode->op.x, node->op.orig_y + snode->op.y);
    //       (*editor)->rectangle(node->op.orig_x + snode->op.x, node->op.orig_y + snode->op.y, snode->op.w, snode->op.h);
    //       (*editor)->clip();
    //       (*editor)->paint();
    //       (*editor)->restore();
    //
    //       //delete sfrm;
    //     }
    //   }
    //   if (!parent_editor.has_value()) {
    //     delete editor;
    //   }
    //   return empty ? nullptr : frm;
    // }

  public
  :
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
        // SDL_GetRendererOutputSize(rtarget->renderer, &dst.w, &dst.h);
        // SDL_Rect window_rect {
        //   .x = 0,
        //   .y = 0,
        //   .w = root->op.w,
        //   .h = root->op.h
        // };
        // SDL_RenderSetViewport(rtarget->renderer, &window_rect);
        // SDL_RenderSetClipRect(rtarget->renderer, &dst);
        // SDL_RenderSetLogicalSize(rtarget->renderer, root->op.w, root->op.h);
        //rtarget->staging_target->width(), rtarget->staging_target->height());
        // SDL_UpdateWindowSurface(rtarget->window);

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
        SDL_RenderCopy(rtarget->renderer, texture, &src, &src);//&src, &dst);
        SDL_RenderPresent(rtarget->renderer);

        // graphics::flush(rtarget);
      }
    }
  };
}
