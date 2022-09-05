//
// Created by castle on 8/31/22.
//

#ifndef CYD_UI_BUTTON_HPP
#define CYD_UI_BUTTON_HPP

#include <functional>
#include "../../include/cydui.hpp"
#include "../../src/logging/logging.hpp"

class ButtonState: public cydui::components::ComponentState {
public:
  //=== CHILDREN STATE DECLARATIONS
  bool hovering = false;
  
  cydui::layout::fonts::Font font {
      .name = "Fira Code Retina",
      .size = 14
  };
  
  cydui::layout::color::Color* c     = new cydui::layout::color::Color("#FCAE1E");
  cydui::layout::color::Color* c_dim = new cydui::layout::color::Color("#2d2310");
  cydui::layout::color::Color* c1    = new cydui::layout::color::Color("#000000");
};

typedef std::function<void()> Action;

class Button: public cydui::components::Component {
public:
  std::string text;
  
  Action on_action;
  
  explicit Button(ButtonState* _state, std::string text, Action on_action)
      : cydui::components::Component(_state) {
    this->text      = text;
    this->on_action = on_action;
  }
  
  void on_redraw(cydui::events::layout::CLayoutEvent* ev) override {
    auto state = (ButtonState*)this->state;
    
    add(
        {
            new primitives::Rectangle(
                state->hovering? state->c : state->c_dim, 0, 0,
                state->geom.content_w().val(), state->geom.content_h().val(),
                true
            ),
            (new primitives::Text(state->hovering? state->c1 : state->c, &state->font, 0, 0, text))
                ->set_margin(5, 5, 5, 5),
        }
    );
  }
  
  void on_mouse_enter(cydui::events::layout::CLayoutEvent* ev) override {
    auto state = (ButtonState*)this->state;
    state->hovering = true;
    state->dirty();
    ev->consumed = true;
  }
  
  void on_mouse_exit(cydui::events::layout::CLayoutEvent* ev) override {
    auto state = (ButtonState*)this->state;
    state->hovering = false;
    state->dirty();
    ev->consumed = true;
  }
  
  void on_mouse_click(cydui::events::layout::CLayoutEvent* ev) override {
    auto state = (ButtonState*)this->state;
    state->hovering = false;
    state->dirty();
    ev->consumed = true;
    this->on_action();
  }
};

#endif //CYD_UI_BUTTON_HPP
