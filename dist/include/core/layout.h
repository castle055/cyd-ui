//
// Created by castle on 9/26/23.
//

#ifndef CYD_UI_LAYOUT_H
#define CYD_UI_LAYOUT_H

#include "components.hpp"
#include "../cydstd/logging.hpp"

namespace cydui::window {
    class CWindow;
}

namespace cydui::layout {
    class Layout;
    
    template<typename C>
    requires components::ComponentConcept<C>
    Layout* create(components::c_init_t<C> init);
    
    class Layout {
      window::CWindow* win = nullptr;
      
      compositing::LayoutCompositor compositor {};
      
      components::ComponentState* root_state;
      components::Component* root;
      components::ComponentState* hovering = nullptr;
      components::ComponentState* focused = nullptr;
      
      components::Component* find_by_coords(
        components::Component* c, int x, int y
      );
      
      bool render_if_dirty(components::Component* c);
      
      Layout(components::ComponentState* _root_state, components::Component* _root)
        : root_state(_root_state), root(_root), focused(_root_state) {
        focused->focused = true;
      }
      
      void redraw_component(cydui::components::Component* target);
      
      void recompute_dimensions(cydui::components::Component* start_from);
      
      void recompose_layout();
      
      template<typename C>
      requires components::ComponentConcept<C>
      friend Layout* cydui::layout::create(components::c_init_t<C> init);
    
    public:
      drag_n_drop::dragging_context_t dragging_context {};
      
      void bind_window(window::CWindow* _win);
    };
}

template<typename C>
requires cydui::components::ComponentConcept<C>
cydui::layout::Layout* cydui::layout::create(components::c_init_t<C> init) {
  auto* root_state = new typename C::State();
  auto* root = new C(
    root_state,
    init.props,
    [init](cydui::components::Component* __raw_local_) {
      auto* local = (C*) __raw_local_;
      
      auto component_name = str("LAYOUT::ROOT<").append(C::Name).append(">");
      logging::logger log = {.name = component_name.c_str(), .on = true, .min_level = logging::WARN};
      auto warn_if_set = [log]<typename T>(std::optional<T> opt) {
        if (opt.has_value()) {
          log.warn("Geometric constraints are ignored for the root component.");
        }
      };
      warn_if_set(init.x);
      warn_if_set(init.y);
      warn_if_set(init.w);
      warn_if_set(init.h);
      
      local->add(init.inner);
      init.init(local);
    }
  );
  auto* lay = new Layout(root_state, root);
  return lay;
}

#endif //CYD_UI_LAYOUT_H
