// Copyright (c) 2025, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include <cairomm-1.16/cairomm/cairomm.h>
#include <tracy/Tracy.hpp>

export module cydui.graphics.compositing_node;

import std;

export import cydui.graphics.vector;
export import cydui.graphics.compositing_operation;
export import cydui.backends.texture_base;

import cydui.application;

export namespace cydui::compositing {
  using namespace vg;
  using namespace std::chrono_literals;

  class compositing_node_t {
  public:
    using sptr = std::shared_ptr<compositing_node_t>;

  private:
    compositing_node_t* parent_;
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

    compositing_operation_t op{};

  public:
    bool       graphics_dirty_    = true;
    bool       rendering_         = false;
    bool       compositing_dirty_ = true;
    fragment_t background_graphics{};
    fragment_t graphics{};

    backends::texture_base::sptr rendered_texture;
    backends::texture_base::sptr composite_texture;
    pixel_t*                     pixels{nullptr};

    explicit compositing_node_t(const sptr& parent)
        : parent_(parent.get()) {
      update_placement();
    }

  public:
    void queue_graphics_update() {
      graphics_dirty_ = true;
    }

    bool is_out_of_bounds() {
      if (nullptr != parent_) {
        auto& pop             = parent_->op;
        bool  x_out_of_bounds = pop.orig_x + op.x > pop.w or pop.orig_x + op.x < -op.w;
        bool  y_out_of_bounds = pop.orig_y + op.y > pop.h or pop.orig_y + op.y < -op.h;
        if (pop.x_overflow == overflow_e::GROW) {
          x_out_of_bounds = false;
        }
        if (pop.y_overflow == overflow_e::GROW) {
          y_out_of_bounds = false;
        }
        if (op.x_position == position_e::ABSOLUTE) {
          x_out_of_bounds = false;
        }
        if (op.y_position == position_e::ABSOLUTE) {
          y_out_of_bounds = false;
        }
        return x_out_of_bounds or y_out_of_bounds;
      }
      return false;
    }

    void update_operation(compositing_operation_t&& new_op) {
      if (new_op != op) {
        op = new_op;
      }
      update_placement();
    }

    void begin_render(const backends::frame_base::sptr& frame) {
      if (graphics_dirty_ or is_dirty_from_flattening()) {
        rendering_ = true;
        if (is_flattened) {
          if (nullptr != rendered_texture) {
            rendered_texture.reset();
          }

          pixel_stride = flattening_target->pixel_stride;
          auto pixel_offset =
            (parent_->op.orig_x + op.x) + (pixel_stride >> 2) * (parent_->op.orig_y + op.y);
          // LOG::print {INFO}("pixel_offset: {}", pixel_offset);
          pixels = &parent_->pixels[pixel_offset];
        } else {
          if (nullptr == rendered_texture) {
            rendered_texture = frame->make_texture();
          }

          if (op.w == 0 or op.h == 0) {
            frame->get_renderer()->resize_texture(rendered_texture, 1, 1);
          } else if (op.w != rendered_texture->width() || op.h != rendered_texture->height()) {
            frame->get_renderer()->resize_texture(rendered_texture, op.w, op.h);
          }

          auto [ptr, pitch] = rendered_texture->begin_render();
          pixel_stride      = pitch;
          pixels            = static_cast<pixel_t*>(ptr);
        }
      }
    }

    void end_render() {
      ZoneScopedN("End Render");
      if (rendering_) {
        rendering_      = false;
        graphics_dirty_ = false;
        if (not is_flattened) {
          ZoneScopedN("Flush Rendered Texture");
          rendered_texture->end_render();
          flattening_dirty = false;
        }
      }
    }

    void compose_own(const backends::frame_base::sptr& frame) {
      if (is_flattened) {
        if (nullptr != composite_texture) {
          composite_texture.reset();
        }
      } else {
        if (nullptr == composite_texture) {
          composite_texture = frame->make_texture(true);
        }

        backends::renderer_base::rect dst{
          .x = 0,
          .y = 0,
          .w = rendered_texture->width(),
          .h = rendered_texture->height(),
        };
        auto renderer = frame->get_renderer();
        renderer->resize_texture(composite_texture, dst.w, dst.h);
        // rendered_texture.swap(composite_texture);
        renderer->copy_texture(rendered_texture, composite_texture, &dst, &dst, false);
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

    void render() {
      if (not rendering_) {
        return;
      }
      compositing_dirty_ = true;

      if (op.w == 0 or op.h == 0) {
        return;
      }
      ZoneScopedN("Render Node");

      pixelmap_editor_t editor{
        flatten_vbox_x + flatten_vbox_w, flatten_vbox_y + flatten_vbox_h, pixels, pixel_stride
      };

      if (not is_flattened) {
        editor.clear();
      }
      editor->begin_new_path();
      editor->rectangle(flatten_vbox_x, flatten_vbox_y, flatten_vbox_w, flatten_vbox_h);
      editor->clip();

      if (not background_graphics.empty()) {
        for (const auto& element: background_graphics.elements) {
          element->_internal_set_origin(0, 0);
          element->apply_to(editor);
        }
      }
      if (not graphics.empty()) {
        for (const auto& element: graphics.elements) {
          element->_internal_set_origin(0, 0); // op.orig_x, op.orig_y);
          element->apply_to(editor);
        }
      }

      //* DEBUG

      // editor->reset_clip();
      // vg::rectangle{}
      //   .x(0)
      //   .y(0)
      //   .w(op.w - 1)
      //   .h(op.h - 1)
      //   .stroke("#00ff0055"_color)
      //   .stroke_width(1)
      //   .stroke_dasharray({5, 5})
      //   .apply_to(editor);
      // vg::rectangle{}
      //   .x(flatten_vbox_x)
      //   .y(flatten_vbox_y)
      //   .w(flatten_vbox_w - 1)
      //   .h(flatten_vbox_h - 1)
      //   .stroke(is_flattened ? "#0000ff"_color : "#00ff00"_color)
      //   .stroke_width(1)
      //   .apply_to(editor);

      // vg::rect { }.w(op.w-1).h(op.h-1)
      //             .stroke(is_flattened ? "#0000ff"_color : "#00ff00"_color)
      //             .stroke_width(1)
      //             .apply_to(editor);
      //
      // if (op.animated) {
      //   vg::rect{}
      //     .x(5).y(5)
      //     .w(op.w - 1 - 10)
      //     .h(op.h - 1 - 10)
      //     .stroke("#ff0000"_color)
      //     .stroke_width(3)
      //     .apply_to(editor);
      // }
      //
    }

    void compose_into(
      const backends::renderer_base::sptr& renderer,
      const backends::texture_base::sptr&  texture
    ) {
      ZoneScopedN("Compose Child");
      float w_ = composite_texture->width();
      float h_ = composite_texture->height();

      backends::renderer_base::rect src{
        .x = static_cast<float>(flatten_vbox_x),
        .y = static_cast<float>(flatten_vbox_y),
        .w = static_cast<float>(flatten_vbox_w),
        .h = static_cast<float>(flatten_vbox_h),
      };
      backends::renderer_base::rect dst{
        .x = static_cast<float>(op.x_position == position_e::ABSOLUTE? op.x: flatten_x),
        .y = static_cast<float>(op.y_position == position_e::ABSOLUTE? op.y: flatten_y),
        .w = static_cast<float>(flatten_vbox_w),
        .h = static_cast<float>(flatten_vbox_h),
      };

      float new_w = texture->width();
      float new_h = texture->height();

      if (op.x_overflow == overflow_e::GROW) {
        new_w = std::max(texture->width(), flatten_x + w_);
      } else {
        if (dst.x > texture->width() or dst.x < -w_) {
          return;
        }
        dst.x += flatten_vbox_x;
      }

      if (op.y_overflow == overflow_e::GROW) {
        new_h = std::max(texture->height(), flatten_y + h_);
      } else {
        if (dst.y > texture->height() or dst.y < -w_) {
          return;
        }
        dst.y += flatten_vbox_y;
      }

      renderer->resize_texture(texture, new_w, new_h, true);
      renderer->copy_texture(composite_texture, texture, &src, &dst, true);
    }

  private:
    void update_placement() {
      bool pos_is_relative = (op.x_position == position_e::RELATIVE and op.y_position == position_e::RELATIVE);
      is_flattened =
        (not op.animated) and ((op.op == compositing_operation_t::OVERLAY) and (parent_ != nullptr))
        and pos_is_relative;
      // and (op.x >= 0) && (op.y >= 0)
      // and ((op.x + op.w) <= parent->op.w)
      // and ((op.y + op.h) <= parent->op.h));

      if (is_flattened) {
        auto rel_x        = parent_->op.orig_x + op.x;
        auto rel_y        = parent_->op.orig_y + op.y;
        flattening_target = parent_->flattening_target;
        flatten_x         = parent_->flatten_x + rel_x;
        flatten_y         = parent_->flatten_y + rel_y;
      } else {
        flattening_target = this;
        flatten_x         = 0;
        flatten_y         = 0;
      }

      if ((nullptr == parent_) or not pos_is_relative) {
        flatten_vbox_x = 0;
        flatten_vbox_y = 0;
        flatten_vbox_w = op.w;
        flatten_vbox_h = op.h;
      } else {
        auto rel_x     = parent_->op.orig_x + op.x;
        auto rel_y     = parent_->op.orig_y + op.y;
        auto clip_x    = std::max(parent_->op.vx, parent_->flatten_vbox_x);
        auto clip_y    = std::max(parent_->op.vy, parent_->flatten_vbox_y);
        flatten_vbox_x = std::max(0, -(rel_x - clip_x));
        flatten_vbox_y = std::max(0, -(rel_y - clip_y));
        auto avail_w   = parent_->op.vw //
                       - std::max(0, parent_->flatten_vbox_x - parent_->op.vx)
                       - std::max(
                         0,
                         ((parent_->op.vw + parent_->op.vx) //
                          - (parent_->flatten_vbox_x + parent_->flatten_vbox_w))
                       );
        auto avail_h = parent_->op.vh //
                       - std::max(0, parent_->flatten_vbox_y - parent_->op.vy)
                       - std::max(
                         0,
                         ((parent_->op.vh + parent_->op.vy) //
                          - (parent_->flatten_vbox_y + parent_->flatten_vbox_h))
                       );
        if (rel_x < clip_x) {
          flatten_vbox_w = std::min(rel_x - clip_x + op.w, avail_w);
        } else {
          flatten_vbox_w = std::min(op.w, avail_w - (rel_x - clip_x));
        }
        if (rel_y < clip_y) {
          flatten_vbox_h = std::min(rel_y - clip_y + op.h, avail_h);
        } else {
          flatten_vbox_h = std::min(op.h, avail_h - (rel_y - clip_y));
        }
      }
    }
  };
} // namespace cydui::compositing
