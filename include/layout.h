//
// Created by castle on 8/21/22.
//

#ifndef CYD_UI_LAYOUT_H
#define CYD_UI_LAYOUT_H


#include "components.h"
#include "window_types.h"

namespace cydui::layout {
  class Layout {
    const window::CWindow* win = nullptr;
    
    components::Component* root;
  public:
    explicit Layout(components::Component* root);
    void bind_window(window::CWindow* win);
    
    void on_event(events::layout::CLayoutEvent* ev);
  };

}


#endif //CYD_UI_LAYOUT_H
