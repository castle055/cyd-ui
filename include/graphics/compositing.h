// Copyright (c) 2024, Victor Castillo, All rights reserved.

#ifndef CYD_UI_COMPOSITING_H
#define CYD_UI_COMPOSITING_H

#include <vector>
#include <deque>
#include <condition_variable>

#include "vg.h"
#include "../cydstd/profiling.h"

namespace cydui::compositing {
    using namespace graphics::vg;
    using namespace std::chrono_literals;
    
    struct compositing_operation_t {
      int x, y, orig_x, orig_y, w, h;
      double rot = 0.0;
      double scale_x = 1.0;
      double scale_y = 1.0;
      
      std::function<void(compositing_operation_t &op)> _fix_dimensions = [](compositing_operation_t &op) { };
    };
    
    struct compositing_object_t {
      compositing_operation_t op {};
      std::vector<vg_element_t> graphics {};
    };
    
    struct compositing_node_t {
      unsigned long id = 0;
      compositing_operation_t op {};
      std::deque<std::unique_ptr<compositing_node_t>> children {};
      vg_fragment_t graphics {};
      
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
            auto* res = item->find_by_id(_id);
            if (nullptr != res) {
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
      std::mutex m;
      std::condition_variable cv;
      std::atomic_flag running {};
      std::thread compositing_thd;
      
      graphics::window_t* render_target = nullptr;
      prof::context_t* profiler = nullptr;
      
      compositing_tree_t* tree = new compositing_tree_t;
      bool tree_dirty = true;
      
      std::unordered_map<unsigned long, pixelmap_t*> sub_frame_cache {};
      
      static void compositing_task(LayoutCompositor* compositor) {
        auto t0 = std::chrono::system_clock::now();
        auto t1 = std::chrono::system_clock::now();
        while (compositor->running.test()) {
          std::unique_lock lk(compositor->m);
          if (nullptr != compositor->render_target) {
            compositor->cv.wait(lk, [&] {
              return compositor->tree_dirty || !compositor->running.test();
            });
            auto _pev = compositor->profiler->scope_event("COMPOSITING TASK");
            compositor->tree_dirty = false;
            
            // Resize if needed, window size -(EV)-> layout size -> frame size -> screen size
            graphics::resize(compositor->render_target, compositor->tree->root->op.w, compositor->tree->root->op.h);
            pixelmap_t* frame = graphics::get_frame(compositor->render_target);
            compositor->repaint(compositor->tree->root, frame);
            graphics::flush(compositor->render_target);
          }
          lk.unlock();
          
          std::this_thread::sleep_until(t0 + 16666us);
        }
      }
      
      pixelmap_t* repaint(std::unique_ptr<compositing_node_t>& node, pixelmap_t* frame = nullptr) {
        pixelmap_t* frm = frame;
        if (nullptr == frm) {
          if (sub_frame_cache.contains(node->id)) {
            frm = sub_frame_cache[node->id];
            if (frm->width() != (size_t) node->op.w || frm->height() != (size_t) node->op.h) {
              frm->resize({
                (unsigned long) node->op.w,
                (unsigned long) node->op.h
              });
            }
          } else {
            frm = new pixelmap_t {
              (unsigned long) node->op.w,
              (unsigned long) node->op.h
            };
            sub_frame_cache[node->id] = frm;
          }
        }
        pixelmap_editor_t editor {frm};
        editor.clear();
        bool empty = true;
        if (!node->graphics.empty()) {
          empty = false;
          // Rasterize graphics into `frm`
          for (const auto &element: node->graphics.elements) {
            element->_internal_set_origin(node->op.orig_x, node->op.orig_y);
            element->apply_to(editor);
          }
        }
        if (!node->children.empty()) {
          empty = false;
          std::vector<std::pair<std::unique_ptr<compositing_node_t>&, pixelmap_t*>> sub_frames {};
          for (auto& c: node->children) {
            sub_frames.emplace_back(c, repaint(c));
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
              (int) sfrm->width(), (int) sfrm->height(),
              cairo_format_stride_for_width(CAIRO_FORMAT_ARGB32, (int) sfrm->width())
            );
            editor->set_source(surface, node->op.orig_x + snode->op.x, node->op.orig_y + snode->op.y);
            editor->paint();
            
            //delete sfrm;
          }
          
        }
        return empty ? nullptr : frm;
      }
    
    public:
      LayoutCompositor() {
        running.test_and_set();
        compositing_thd = std::thread {&compositing_task, this};
        pthread_t pt = compositing_thd.native_handle();
        pthread_setname_np(pt, "COMPOSITING_THD");
      }
      
      ~LayoutCompositor() {
        running.clear();
        cv.notify_all();
        compositing_thd.join();
      }
      
      void set_render_target(graphics::window_t* _render_target, prof::context_t* _profiler) {
        std::lock_guard lk(m);
        render_target = _render_target;
        profiler = _profiler;
      }
      
      void compose(compositing_tree_t* _tree) {
        compositing_tree_t* old_tree;
        {
          std::lock_guard lk(m);
          old_tree = tree;
          tree = _tree;
          tree_dirty = true;
        }
        cv.notify_all();
        delete old_tree;
      }
    };
}

#endif //CYD_UI_COMPOSITING_H
