//
// Created by castle on 7/17/23.
//

#ifndef CYD_UI_DRAG_N_DROP_H
#define CYD_UI_DRAG_N_DROP_H

#include <functional>

namespace cyd::ui::drag_n_drop {
    struct draggable_t {
      void* data = nullptr;
      
      std::function<void(draggable_t, int, int)> drag_move = [](draggable_t, int, int) {
      };
      std::function<void(draggable_t, int, int)> drag_end = [](draggable_t, int, int) {
      };
    };
    
    struct draggable_source_t {
      int x = 0;
      int y = 0;
      
      draggable_t start_drag(int _x, int _y) {
        draggable_t d = {
          .drag_move = this->drag_move,
          .drag_end = this->drag_end,
        };
        return drag_start(d, _x, _y);
      }
      
      std::function<draggable_t(draggable_t, int, int)> drag_start = [](draggable_t d, int, int) {
        return d;
      };
      std::function<void(draggable_t, int, int)> drag_move = [](draggable_t, int, int) {
      };
      std::function<void(draggable_t, int, int)> drag_end = [](draggable_t, int, int) {
      };
    };
    
    struct dragging_context_t {
      draggable_t dragging_item {};
      bool dragging = false;
      
      void set_item(draggable_t item) {
        dragging_item = item;
        dragging = true;
      }
      
      void set_item_if_empty(draggable_t item) {
        if (!dragging) set_item(std::move(item));
      }
    };
}

#endif //CYD_UI_DRAG_N_DROP_H
