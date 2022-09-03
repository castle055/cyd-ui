//
// Created by castle on 8/31/22.
//

#ifndef CYD_UI_BUTTON_H
#define CYD_UI_BUTTON_H

#include <functional>
#include "../../include/cydui.h"
#include "../../src/logging/logging.h"

class ButtonState: public cydui::components::ComponentState {
public:
  //=== CHILDREN STATE DECLARATIONS
  bool hovering = false;
  
  cydui::layout::fonts::Font font {
      .name = "Fira Code Retina",
      .size = 14
  };
  
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
    auto* c     = new cydui::layout::color::Color("#FCAE1E");
    auto* c_dim = new cydui::layout::color::Color("#2d2310");
    auto* c1    = new cydui::layout::color::Color("#000000");
    
    add(
        {
            new primitives::Rectangle(
                state->hovering? c : c_dim, 0, 0,
                state->geom.content_w(), state->geom.content_h(),
                true
            ),
            (new primitives::Text(state->hovering? c1 : c, &state->font, 0, 0, text))
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

#endif //CYD_UI_BUTTON_H
