//
// Created by castle on 10/15/23.
//

#ifndef CYD_UI_COMPOSITING_H
#define CYD_UI_COMPOSITING_H

#include <vector>
#include <deque>
#include <condition_variable>

#include "vg.h"

namespace cydui::compositing {
    using namespace vg;
    
    struct compositing_operation_t {
      int x, y, w, h;
      double rot = 0.0;
      double scale_x = 1.0;
      double scale_y = 1.0;
    };
    
    struct compositing_object_t {
      compositing_operation_t op {};
      std::vector<vg_element_t> graphics {};
    };
    
    struct compositing_node_t {
      compositing_operation_t op {};
      std::deque<compositing_node_t*> children {};
      vg_fragment_t graphics {};
      
      ~compositing_node_t() {
        for (auto &item: children) {
          delete item;
        }
      }
    };
    
    struct compositing_tree_t {
      compositing_node_t root {};
    };
    
    class LayoutCompositor {
      std::mutex m;
      std::condition_variable cv;
      std::atomic_flag running {};
      std::thread compositing_thd;
      
      graphics::window_t* render_target = nullptr;
      
      compositing_tree_t* tree = new compositing_tree_t;
      
      static void compositing_task(LayoutCompositor* compositor) {
        while (true) {//compositor->running.test()) {
          std::unique_lock lk(compositor->m);
          compositor->cv.wait(lk, [] {
            return true;
          });
          
          if (compositor->render_target) {
            // Resize if needed, window size -(EV)-> layout size -> frame size -> screen size
            graphics::resize(compositor->render_target, compositor->tree->root.op.w, compositor->tree->root.op.h);
            pixelmap_t* frame = graphics::get_frame(compositor->render_target);
            compositor->repaint(&compositor->tree->root, frame);
            graphics::flush(compositor->render_target);
          }
          
          lk.unlock();
        }
      }
      
      pixelmap_t* repaint(compositing_node_t* node, pixelmap_t* frame = nullptr) {
        if (!node->children.empty()) {
          std::vector<std::pair<compositing_node_t*, pixelmap_t*>> sub_frames {};
          for (auto* c: node->children) {
            sub_frames.emplace_back(c, repaint(c));
          }
          pixelmap_t* frm = frame;
          if (nullptr == frm) {
            frm = new pixelmap_t {
              (unsigned long) node->op.w,
              (unsigned long) node->op.h
            };
          }
          
          // Compose graphics into 'frm'
          pixelmap_editor_t editor {frm};
          
          for (const auto &item: sub_frames) {
            auto &[snode, sfrm] = item;
            auto surface = Cairo::ImageSurface::create(
              (unsigned char*) sfrm->data,
              Cairo::Surface::Format::ARGB32,
              (int) sfrm->width(), (int) sfrm->height(),
              cairo_format_stride_for_width(CAIRO_FORMAT_ARGB32, (int) sfrm->width())
            );
            editor->set_source(surface, snode->op.x, snode->op.y);
            editor->paint_with_alpha(1.0);
            
            delete sfrm;
          }
          
          return frm;
        } else if (!node->graphics.empty()) {
          pixelmap_t* frm = frame;
          if (nullptr == frm) {
            frm = new pixelmap_t {
              (unsigned long) node->op.w,
              (unsigned long) node->op.h
            };
          }
          // Rasterize graphics into `frm`
          pixelmap_editor_t editor {frm};
          for (const auto &element: node->graphics.elements) {
            element->apply_to(editor);
          }
          
          return frm;
        } else {
          return nullptr;
          // ! If this happens, I'm okay with everything burning down
        }
      }
    
    public:
      LayoutCompositor() {
        running.test_and_set();
        compositing_thd = std::thread {&compositing_task, this};
      }
      
      ~LayoutCompositor() {
        running.clear();
        compositing_thd.join();
      }
      
      void set_render_target(graphics::window_t* _render_target) {
        std::lock_guard lk(m);
        render_target = _render_target;
      }
      
      void compose(compositing_tree_t* _tree) {
        compositing_tree_t* old_tree;
        {
          std::lock_guard lk(m);
          old_tree = tree;
          tree = _tree;
        }
        cv.notify_all();
        delete old_tree;
      }
    };
}

#endif //CYD_UI_COMPOSITING_H
