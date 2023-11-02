//
// Created by castle on 10/26/23.
//

#ifndef CYD_UI_COMPONENT_H
#define CYD_UI_COMPONENT_H

#include "component_event_handler.h"
#include "component_attributes.h"
#include "cydstd/lazy_alloc.h"

#include "component_macros.h"
#include "with_specialization.h"

#include "../graphics/compositing.h"

#include <optional>

struct component_state_t {
  std::optional<component_state_t*> parent = std::nullopt;
  std::unordered_map<std::string, component_state_t*> children_states {};
};

struct component_base_t {
  std::optional<component_state_t*> state = std::nullopt;
  std::optional<component_base_t*> parent = std::nullopt;
  std::vector<component_base_t*> children {};
  
  vg::vg_fragment_t fragment {};
  
  std::vector<cydui::events::listener_t> subscribed_listeners {};
  
  ~component_base_t() {
    clear_subscribed_listeners();
  }
  virtual void subscribe_events() = 0;
  
  virtual event_handler_t* event_handler() = 0;
  
  virtual void redraw() = 0;
  virtual void generate_fragment() = 0;
  
  virtual component_state_t* create_state_instance() = 0;
  
  virtual component_base_t* find_by_coords(int x, int y) = 0;

protected:
  void add_event_listeners(const std::unordered_map<std::string, event_handler_t::listener_data_t> &listeners) {
    for (auto &l: listeners) {
      auto &[ev_type, handler] = l;
      subscribed_listeners.push_back(cydui::events::on_event_raw(ev_type, handler.handler));
    }
  }
private:
  void clear_subscribed_listeners() {
    for (auto &item: subscribed_listeners) {
      item.remove();
    }
    subscribed_listeners.clear();
  }
};

template<ComponentEventHandlerConcept EVH, typename T>
struct component_t:
  public component_base_t,
  attr_x<T>,
  attr_y<T>,
  attr_w<T>,
  attr_h<T> {
private:
  lazy_alloc<EVH> event_handler_ {};

public:
  void subscribe_events() override {
    if (subscribed_listeners.empty()) {
      add_event_listeners(event_handler_->get_event_listeners());
    }
  }
  component_state_t* create_state_instance() override {
    return new typename T::state_t;
  }
  event_handler_t* event_handler() override {
    return event_handler_;
  }
  
  void redraw() override {
    std::vector<component_holder_t> new_children = event_handler_->on_redraw();
    
    std::size_t id_i = 0;
    for (auto &item: new_children) {
      for (auto &component_pair: item.get_components()) {
        auto [id_, component] = component_pair;
        std::string id = id_;
        id.append(":");
        id.append(std::to_string(id_i));
        
        // Get or Create state for component
        component_state_t* child_state;
        if (state.value()->children_states.contains(id)) {
          child_state = state.value()->children_states[id];
        } else {
          child_state = component->create_state_instance();
          state.value()->children_states[id] = child_state;
        }
        
        // Set child's variables
        component->state = child_state;
        child_state->parent = state.value();
        component->parent = this;
        children.push_back(component);
        
        // Subscribe child events
        component->subscribe_events();
        
        // Redraw child
        component->redraw();
      }
      ++id_i;
    }
    
    generate_fragment();
  }
  
  void generate_fragment() override {
    fragment.clear();
    
  }
  
  component_base_t* find_by_coords(int x, int y) override {
  
  }
};

//#include "../graphics/vg.h"
//
//struct CanvasState: public component_state_t {
//
//};
//
//struct CanvasEventHandler: public event_handler_t {
//  CanvasState* state;
//  int* props;
//
//  std::vector<component_holder_t> on_redraw() final {
//    return {};
//  }
//};
//
//struct Canvas
//  : public component_t<CanvasEventHandler, Canvas> {
//  CYDUI_COMPONENT_METADATA(Canvas)
//  using state_t = CanvasState;
//  using event_handler_t = CanvasEventHandler;
//  struct props_t {
//
//  } props {};
//
//  Canvas() = default;
//  explicit Canvas(props_t props): props(props) { }
//
//  Canvas &operator()(const std::function<vg::vg_fragment_t()> &render) {
//    auto frag = render();
//    return *this;
//  }
//};
//
//auto test(auto a, auto b) {
//  return a + b;
//}
//
//void sadfasdfasddf() {
//  auto i = test(2, 4.0);
//  auto j = test("asd", 'a');
//}

struct $ {
  $(int a, float f) {
  
  };
};

constexpr long operator "" _px(unsigned long long val) {
  return val;
}

using namespace std::chrono_literals;
void tasdfest() {
  $ {2, 1.4f};
  1ms;
  10_px;
}

#endif //CYD_UI_COMPONENT_H
