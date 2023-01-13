//
// Created by castle on 1/4/23.
//

#ifndef CYD_UI_LISTVIEW_HPP
#define CYD_UI_LISTVIEW_HPP

#include "../../../include/cydui.hpp"
#include "../../../include/logging.hpp"

STATE(ListView)
  int diff = 0;
  
  INIT_STATE(ListView) {
  }
};

COMPONENT(ListView)
  PROPS({
    int* scroll = new int(0);
    int spacing = 40;
  })
  
  INIT(ListView)
  }
  
  REDRAW {
    WITH_STATE(ListView)
    
    *props.scroll += state->diff;
    state->diff = 0;
    //logger.info("SCROLL= %d", props.scroll);
    int x = 0;
    int y = *props.scroll + props.spacing;
    
    auto given_children = children;
    children.clear();
    std::vector<Component*> items;
    for (const auto         &item: given_children) {
      int item_height = item->state->geom.h.val();
      item->set_pos(this, x, y);
      // Push only if in viewport
      if (y <= 800 && y + item_height + props.spacing >= 0) {
        children.push_back(item);
      }
      y += props.spacing + item_height;
    }
    if (y < 400 && *props.scroll < -400) {
      state->diff = 400 - y;
    }
  }
};

#endif //CYD_UI_LISTVIEW_HPP
