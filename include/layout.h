// Copyright (c) 2024, Victor Castillo, All rights reserved.

#ifndef CYD_UI_LAYOUT_H
#define CYD_UI_LAYOUT_H

#include "components/component.h"
#include "cydstd/logging.hpp"

namespace cyd::ui::window {
    class CWindow;
}

namespace cyd::ui::layout {
    class Layout;
    
    template<components::ComponentConcept C>
    Layout* create(C &&root_component);
    
    class Layout {
      window::CWindow* win = nullptr;
      
      compositing::LayoutCompositor compositor {};
      
      components::component_state_ref root_state;
      components::component_base_t* root;
      
      components::component_state_ref hovering = nullptr;
      components::component_state_ref focused = nullptr;
      
      std::vector<cyd::fabric::async::listener_t*> listeners {};
      
      components::component_base_t* find_by_coords(int x, int y);
      
      Layout(const components::component_state_ref& _root_state, components::component_base_t* _root)
        : root_state(_root_state), root(_root), focused(_root_state) {
        focused->focused = true;
      }
      
      bool render_if_dirty(components::component_base_t* c);
      
      void redraw_component(components::component_base_t* target);
      
      void recompute_dimensions(components::component_base_t* start_from);
      
      void recompose_layout();
      
      template<components::ComponentConcept C>
      friend Layout* cyd::ui::layout::create(C &&root_component);
    
    public:
      ~Layout() {
        for (auto &item: listeners) {
          item->remove();
          delete item;
        }
      }
      
      //drag_n_drop::dragging_context_t dragging_context {};
      
      void bind_window(window::CWindow* _win);
    };
}

template<cyd::ui::components::ComponentConcept C>
inline cyd::ui::layout::Layout* cyd::ui::layout::create(C &&root_component) {
  auto root_state = C::make_state_instance();
  auto* root = new C {root_component};
  root->state = root_state;
  auto* lay = new Layout(root_state, root);
  return lay;
}

#endif //CYD_UI_LAYOUT_H
