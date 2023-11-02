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

#include <format>

struct component_base_t {
  virtual event_handler_t* event_handler() = 0;
};

template<ComponentEventHandlerConcept EVH, typename T>
struct component_t:
  public component_base_t,
  attr_x<T>,
  attr_y<T>,
  attr_w<T>,
  attr_h<T> {
private:
  std::vector<cydui::events::listener_t> subscribed_listeners {};
  void clear_subscribed_listeners() {
    for (auto &item: subscribed_listeners) {
      item.remove();
    }
    subscribed_listeners.clear();
  }
  
  lazy_alloc<EVH> event_handler_ {};
  
  void test() {
    event_handler_->on_redraw();
    
    auto listeners = event_handler_->get_event_listeners();
    for (auto &l: listeners) {
      auto &[ev_type, handler] = l;
      subscribed_listeners.push_back(cydui::events::on_event_raw(ev_type, handler));
    }
  }

public:
  event_handler_t* event_handler() override {
    return event_handler_;
  }
};

#include "../graphics/vg.h"

struct CanvasState: public component_state_t {

};

struct CanvasEventHandler: public event_handler_t {
  CanvasState* state;
  int* props;
  
  std::vector<component_holder_t> on_redraw() final {
    return {};
  }
};

struct Canvas
  : public component_t<CanvasEventHandler, Canvas> {
  CYDUI_COMPONENT_METADATA(Canvas)
  using state_t = CanvasState;
  using event_handler_t = CanvasEventHandler;
  struct props_t {
  
  } props {};
  
  Canvas() = default;
  explicit Canvas(props_t props): props(props) { }
  
  Canvas &operator()(const std::function<vg::vg_fragment_t()> &render) {
    auto frag = render();
    return *this;
  }
};

auto test(auto a, auto b) {
  return a + b;
}

void sadfasdfasddf() {
  auto i = test(2, 4.0);
  auto j = test("asd", 'a');
}

#endif //CYD_UI_COMPONENT_H
