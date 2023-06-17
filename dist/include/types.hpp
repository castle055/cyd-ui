//
// Created by castle on 2/16/23.
//

#ifndef CYD_UI_TYPES_HPP
#define CYD_UI_TYPES_HPP

#include <optional>
#include <functional>
#include <string>

#include <cyd-log/dist/include/logging.hpp>

#include "cydstd/cydstd.h"

#include "dimensions.hpp"
#include "children_state_collection.hpp"
#include "x11_impl.hpp"

namespace cydui {
    namespace components {
        class Component;
        
        class ComponentState;
        
        class ComponentBorder;
        
        template<class c>
        concept ComponentConcept = requires {
          typename c::Props;
          typename c::State;
          {
          c::props
          } -> std::convertible_to<typename c::Props>;
          {
          c::Name
          } -> std::convertible_to<const char*>;
        };
        
        struct component_builder_t {
          // Geometric constraints
          std::optional<cydui::dimensions::dimensional_relation_t> x;
          std::optional<cydui::dimensions::dimensional_relation_t> y;
          std::optional<cydui::dimensions::dimensional_relation_t> w;
          std::optional<cydui::dimensions::dimensional_relation_t> h;
          
          std::function<Component*(component_builder_t)> build;
        };
        
        template<typename c> requires ComponentConcept<c>
        struct c_init_t {
          typename c::State** ref = nullptr;
          typename c::Props props;
          
          // Geometric constraints
          std::optional<cydui::dimensions::dimensional_relation_t> x;
          std::optional<cydui::dimensions::dimensional_relation_t> y;
          std::optional<cydui::dimensions::dimensional_relation_t> w;
          std::optional<cydui::dimensions::dimensional_relation_t> h;
          
          std::vector<component_builder_t> inner = {};
          
          std::function<void(c*)> init = [](c*) {
          };
        };
    }
    namespace layout {
        class Layout;
        
        template<typename C>
        requires components::ComponentConcept<C>
        Layout* create(components::c_init_t<C> init);
        
        namespace images {
            struct image_t {
              str path;
            };
        }
        namespace fonts {
            struct Font {
              str name;
              int size;
              
              bool antialias = true;
              bool autohint = true;
            };
          
        }
        namespace color {
            class Color {
              str hex;
            
            public:
              explicit Color(str color);
              
              str to_string();
            };
        }
    }
    namespace c {
        extern cydui::layout::color::Color White;
        extern cydui::layout::color::Color Black;
        extern cydui::layout::color::Color Red;
        extern cydui::layout::color::Color Green;
        extern cydui::layout::color::Color Blue;
    }
    namespace window {
        class CWindow;
        
        CWindow* create(
          layout::Layout* layout,
          const char* title = "CydUI",
          const char* wclass = "cydui",
          int x = 0,
          int y = 0,
          int w = 640,
          int h = 480,
          bool override_redirect = false
        );
    }
    namespace graphics {
        typedef window_ti window_t;
        typedef render_target_ti render_target_t;
    }
}

class cydui::components::ComponentBorder {
public:
  bool enabled = false;
  cydui::layout::color::Color* color = new layout::color::Color("#FCAE1E");
};

class cydui::components::ComponentState {
public:
  ComponentState();
  
  Component* component_instance;
  bool stateless_comp = false;
  bool focused = false;
  
  bool _dirty = false;
  
  void dirty();
  
  nullable<cydui::graphics::window_t*> win;
  nullable<cydui::graphics::render_target_t*> sub_render_target;
  std::pair<int, int> sub_render_event_offset = {0, 0};
  
  cydui::dimensions::component_dimensions_t dim;
  ComponentBorder border;
  ChildrenStateCollection children;
};

class cydui::layout::Layout {
  const window::CWindow* win = nullptr;
  
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
  
  template<typename C>
  requires components::ComponentConcept<C>
  friend Layout* cydui::layout::create(components::c_init_t<C> init);

public:
  
  void bind_window(window::CWindow* _win);
};

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

class cydui::window::CWindow {
public:
  graphics::window_t* win_ref;
  layout::Layout* layout;
};

#endif //CYD_UI_TYPES_HPP
