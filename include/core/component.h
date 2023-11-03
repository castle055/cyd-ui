//
// Created by castle on 10/26/23.
//

#ifndef CYD_UI_COMPONENT_H
#define CYD_UI_COMPONENT_H

#include "cydstd/lazy_alloc.h"

#include "component_event_handler.h"
#include "with_specialization.h"

#include "component_attributes.h"
#include "component_macros.h"

#include <optional>

namespace cydui::layout {
    class Layout;
}

struct component_state_t {
  std::optional<component_state_t*> parent = std::nullopt;
  std::unordered_map<std::string, component_state_t*> children_states {};
  
  bool _dirty = false;
  
  bool focused = false;
  bool hovering = false;
  
  std::optional<component_base_t*> component_instance = std::nullopt;
};

struct component_base_t {
  std::optional<component_state_t*> state = std::nullopt;
  std::optional<component_base_t*> parent = std::nullopt;
  std::vector<component_base_t*> children {};
  
  std::vector<cydui::events::listener_t> subscribed_listeners {};
  
  virtual ~component_base_t() {
    clear_subscribed_listeners();
    if (state.has_value()) {
      state.value()->component_instance = std::nullopt;
    }
  }
  virtual void subscribe_events() = 0;
  virtual void clear_children() = 0;
  
  virtual event_handler_t* event_handler() = 0;
  
  virtual void redraw(cydui::layout::Layout* layout, cydui::compositing::compositing_node_t* compositing_node) = 0;
  
  virtual component_state_t* create_state_instance() = 0;
  
  virtual component_base_t* find_by_coords(int x, int y) = 0;
  
  virtual component_dimensional_relations_t get_dimensional_relations() = 0;

protected:
  void add_event_listeners(const std::unordered_map<std::string, event_handler_t::listener_data_t> &listeners) {
    for (auto &l: listeners) {
      auto &[ev_type, handler] = l;
      subscribed_listeners.push_back(cydui::events::on_event_raw(ev_type, handler.handler));
    }
  }
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
  attrs_dimensions<T>,
  attrs_margin<T>,
  attrs_padding<T> {
private: //! Private attributes
  cydui::dimensions::dimension_t cx;
  cydui::dimensions::dimension_t cy;
  cydui::dimensions::dimension_t cw;
  cydui::dimensions::dimension_t ch;
private:
  lazy_alloc<EVH> event_handler_ {};

public:
  ~component_t() override = default;
  
  void subscribe_events() override {
    clear_subscribed_listeners();
    if (parent.has_value()) {
      event_handler_->parent = parent.value()->event_handler();
    } else {
      event_handler_->parent = nullptr;
    }
    event_handler_->state = (typename T::state_t*) state.value();
    event_handler_->props = &(((T*) this)->props);
    event_handler_->get_dim = [this] {return get_dimensional_relations();};
    if (subscribed_listeners.empty()) {
      add_event_listeners(event_handler_->get_event_listeners());
    }
  }
  void clear_children() override {
    for (auto &child: children) {
      delete child;
    }
    children.clear();
  }
  
  component_state_t* create_state_instance() override {
    return new typename T::state_t;
  }
  event_handler_t* event_handler() override {
    return event_handler_;
  }
  
  void redraw(cydui::layout::Layout* layout, cydui::compositing::compositing_node_t* compositing_node) override {
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
        child_state->component_instance = component;
        component->parent = this;
        children.push_back(component);
        
        // Subscribe child events
        component->subscribe_events();
        
        // Redraw child
        auto* child_node = new cydui::compositing::compositing_node_t;
        compositing_node->children.push_back(child_node);
        component->redraw(layout, child_node);
      }
      ++id_i;
    }
    
    
    compositing_node->id = (unsigned long) (this->state.value());
    compositing_node->op = {
      .rot = 0.0, // dim->rot.val(),
      .scale_x = 1.0, // dim->scale_x.val(),
      .scale_y = 1.0, // dim->scale_y.val(),
      ._fix_dimensions = [this](cydui::compositing::compositing_operation_t &op) {
        op.x = this->_x.val();
        op.y = this->_y.val();
        op.w = this->_w.val();
        op.h = this->_h.val();
      }
    };
    
    auto &fragment = compositing_node->graphics;
    fragment.clear();
    event_handler_->draw_fragment(fragment);
  }
  
  component_base_t* find_by_coords(int x, int y) override {
    component_base_t* found = nullptr;
    for (auto c = children.rbegin(); c != children.rend(); ++c) {
      found = (*c)->find_by_coords(x, y);
      if (nullptr != found) {
        return found;
      }
    }
    if (cx.val() <= x && x < cx.val() + cw.val()) {
      if (cy.val() <= y && y < cy.val() + ch.val()) {
        return this;
      }
    }
    return nullptr;
  }
  
  component_dimensional_relations_t get_dimensional_relations() override {
    return {
      this->_x,
      this->_y,
      this->_w,
      this->_h,
      this->_w_has_changed,
      this->_h_has_changed,
      this->cx,
      this->cy,
      this->cw,
      this->ch,
      this->_margin_top,
      this->_margin_bottom,
      this->_margin_left,
      this->_margin_right,
      this->_padding_top,
      this->_padding_bottom,
      this->_padding_left,
      this->_padding_right,
    };
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

//struct $ {
//  $(int a, float f) {
//
//  };
//};
//
//constexpr long operator "" _px(unsigned long long val) {
//  return val;
//}
//
//using namespace std::chrono_literals;
//void tasdfest() {
//  $ {2, 1.4f};
//  1ms;
//  10_px;
//}

#endif //CYD_UI_COMPONENT_H
