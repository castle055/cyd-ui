// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include <cairomm-1.16/cairomm/cairomm.h>

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

    double x, y, orig_x, orig_y, w, h;
    double rot = 0.0;
    double scale_x = 1.0;
    double scale_y = 1.0;

    std::function<void(compositing_operation_t &op)> _fix_dimensions = [](compositing_operation_t &op) {
    };
  };

  struct compositing_node_t {
    unsigned long id = 0;
    compositing_operation_t op{};
    std::deque<std::unique_ptr<compositing_node_t>> children{};
    vg_fragment_t graphics{};

    //void fix_op_dimensions() {
    //  op._fix_dimensions(op);
    //  for (auto &node: children) {
    //    node->fix_op_dimensions();
    //  }
    //}

    compositing_node_t* find_by_id(unsigned long _id) {
      if (id == _id) {
        return this;
      } else {
        for (auto &item: children) {
          if (auto* res = item->find_by_id(_id); nullptr != res) {
            return res;
          }
        }
        return nullptr;
      }
    }
  };

  struct compositing_tree_t {
    std::unique_ptr<compositing_node_t> root = std::make_unique<compositing_node_t>();

    //void fix_dimensions() {
    //  root.fix_op_dimensions();
    //}
    //
    compositing_node_t* find_by_id(unsigned long _id) {
      return root->find_by_id(_id);
    }
  };

  class LayoutCompositor {
    graphics::window_t* render_target = nullptr;
    prof::context_t* profiler = nullptr;

    std::shared_ptr<compositing_tree_t> tree = std::make_shared<compositing_tree_t>();
    bool tree_dirty = true;

    std::unordered_map<unsigned long, pixelmap_t*> sub_frame_cache{};

    pixelmap_t* repaint(
      std::unique_ptr<compositing_node_t> &node,
      pixelmap_t* frame = nullptr,
      std::optional<pixelmap_editor_t*> parent_editor = std::nullopt,
      int x_off_in_parent = 0,
      int y_off_in_parent = 0
    ) {
      pixelmap_editor_t* editor;
      pixelmap_t* frm = frame;
      if (!parent_editor.has_value()) {
        if (nullptr == frm) {
          if (sub_frame_cache.contains(node->id)) {
            frm = sub_frame_cache[node->id];
            if (frm->width() != (size_t) node->op.w || frm->height() != (size_t) node->op.h) {
              frm->resize(
                {
                  (unsigned long) node->op.w,
                  (unsigned long) node->op.h
                }
              );
            }
            editor = new pixelmap_editor_t{frm};
            editor->clear();
          } else {
            frm = new pixelmap_t{
              (unsigned long) node->op.w,
              (unsigned long) node->op.h
            };
            sub_frame_cache[node->id] = frm;
            editor = new pixelmap_editor_t{frm};
          }
        } else {
          editor = new pixelmap_editor_t{frm};
          editor->clear();
        }
      } else {
        editor = parent_editor.value();
      }

      bool empty = true;
      if (!node->graphics.empty()) {
        empty = false;
        // Rasterize graphics into `frm`
        for (const auto &element: node->graphics.elements) {
          element->_internal_set_origin(
            x_off_in_parent + node->op.orig_x,
            y_off_in_parent + node->op.orig_y
          );
          element->apply_to(*editor);
        }
      }
      if (!node->children.empty()) {
        empty = false;
        std::vector<std::pair<std::unique_ptr<compositing_node_t> &, pixelmap_t*>> sub_frames{};
        for (auto &c: node->children) {
          if (c->op.op == compositing_operation_t::OVERLAY) {
            repaint(
              c,
              frm,
              editor,
              x_off_in_parent + node->op.orig_x + c->op.x,
              y_off_in_parent + node->op.orig_y + c->op.y
            );
          } else {
            sub_frames.emplace_back(c, repaint(c));
          }
        }

        // Compose graphics into 'frm'

        for (const auto &item: sub_frames) {
          auto &[snode, sfrm] = item;
          if (nullptr == sfrm) {
            continue;
          }
          auto surface = Cairo::ImageSurface::create(
            (unsigned char*) sfrm->data,
            Cairo::Surface::Format::ARGB32,
            (int) sfrm->width(),
            (int) sfrm->height(),
            (int) sfrm->width() * 4
            //cairo_format_stride_for_width(CAIRO_FORMAT_ARGB32, (int) sfrm->width())
          );
          (*editor)->save();
          // (*editor)->set_antialias(Cairo::ANTIALIAS_NONE);
          (*editor)->set_operator(Cairo::Context::Operator::OVER);
          (*editor)->set_source(surface, node->op.orig_x + snode->op.x, node->op.orig_y + snode->op.y);
          (*editor)->rectangle(node->op.orig_x + snode->op.x, node->op.orig_y + snode->op.y, snode->op.w, snode->op.h);
          (*editor)->clip();
          (*editor)->paint();
          (*editor)->restore();

          //delete sfrm;
        }
      }
      if (!parent_editor.has_value()) {
        delete editor;
      }
      return empty ? nullptr : frm;
    }

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

    void compose(std::shared_ptr<compositing_tree_t> _tree) {
      auto* rtarget = render_target;
      if (_tree != nullptr) {
        size_t w = _tree->root->op.w;
        size_t h = _tree->root->op.h;

        pixelmap_t* frame = graphics::get_frame(rtarget);
        if (w > 0 && h > 0) {
          frame->resize({w, h});
        }
        repaint(_tree->root, frame);

        SDL_DestroyTexture(rtarget->texture);
        rtarget->texture = SDL_CreateTexture(rtarget->renderer, SDL_PIXELFORMAT_BGRA32, SDL_TEXTUREACCESS_STREAMING,
                                             frame->width(), frame->height());
        SDL_UpdateTexture(rtarget->texture, nullptr, frame->data,
                          frame->width() * 4);
        SDL_Rect src {
          .x = 0,
          .y = 0,
          .w = (int)frame->width(),
          .h = (int)frame->height()
        };

        SDL_Rect dst {
          .x = 0,
          .y = 0,
        };
        // SDL_GetRendererOutputSize(rtarget->renderer, &dst.w, &dst.h);
        // SDL_RenderSetViewport(rtarget->renderer, &src);
        // SDL_RenderSetClipRect(rtarget->renderer, &dst);
        // SDL_RenderSetLogicalSize(rtarget->renderer, dst.w, dst.h);
        //rtarget->staging_target->width(), rtarget->staging_target->height());
        // SDL_UpdateWindowSurface(rtarget->window);

        SDL_RenderClear(rtarget->renderer);

        SDL_RenderCopy(rtarget->renderer, rtarget->texture, nullptr, nullptr);//&src, &dst);
        SDL_RenderPresent(rtarget->renderer);

        // graphics::flush(rtarget);
      }
    }
  };
}
