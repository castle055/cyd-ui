//
// Created by castle on 9/26/23.
//

#ifndef CYD_UI_LAYOUT_H
#define CYD_UI_LAYOUT_H

#include "component.h"
#include "../cydstd/logging.hpp"

namespace cydui::window {
    class CWindow;
}

namespace cydui::layout {
    class Layout;
    
    template<components::ComponentConcept C>
    Layout* create(C &&root_component);
    
    class Layout {
      window::CWindow* win = nullptr;
      
      compositing::LayoutCompositor compositor {};
      
      components::component_state_t* root_state;
      components::component_base_t* root;
      
      components::component_state_t* hovering = nullptr;
      components::component_state_t* focused = nullptr;
      
      
      components::component_base_t* find_by_coords(int x, int y);
      
      bool render_if_dirty(components::component_base_t* c);
      
      Layout(components::component_state_t* _root_state, components::component_base_t* _root)
        : root_state(_root_state), root(_root), focused(_root_state) {
        focused->focused = true;
      }
      
      void redraw_component(components::component_base_t* target);
      
      void recompute_dimensions(components::component_base_t* start_from);
      
      void recompose_layout();
      
      template<components::ComponentConcept C>
      friend Layout* cydui::layout::create(C &&root_component);
    
    public:
      //drag_n_drop::dragging_context_t dragging_context {};
      
      void bind_window(window::CWindow* _win);
    };
}

template<cydui::components::ComponentConcept C>
cydui::layout::Layout* cydui::layout::create(C &&root_component) {
  auto* root_state = new typename C::state_t();
  auto* root = new C {root_component};
  root->state = root_state;
  auto* lay = new Layout(root_state, root);
  return lay;
}

#endif //CYD_UI_LAYOUT_H
