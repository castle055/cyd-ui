//
// Created by castle on 1/4/23.
//

#ifndef CYD_UI_LISTVIEW_HPP
#define CYD_UI_LISTVIEW_HPP

#include "../../../include/cydui.hpp"
#include "../../../include/logging.hpp"

STATE(ListView)
  INIT_STATE(ListView) {
  }
};

COMPONENT(ListView)
  PROPS({
    int scroll  = -1;
    int spacing = 40;
  })
  
  INIT(ListView)
  }
  
  REDRAW {
    WITH_STATE(ListView)
    
    //log.info("SCROLL= %d", props.scroll);
    int x = 0;
    int y = props.scroll + props.spacing;
    
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
  }
};

#endif //CYD_UI_LISTVIEW_HPP
