//
// Created by castle on 8/31/22.
//

#ifndef CYD_UI_BUTTON_HPP
#define CYD_UI_BUTTON_HPP

#include <functional>
#include "../../include/cydui.hpp"
#include "../../src/logging/logging.hpp"

STATE(Button)
  bool hovering = false;
  
  cydui::layout::fonts::Font font {
    .name = "Fira Code Retina",
    .size = 14
  };
  
  cydui::layout::color::Color* c     = new cydui::layout::color::Color("#FCAE1E");
  cydui::layout::color::Color* c_dim = new cydui::layout::color::Color("#2d2310");
  cydui::layout::color::Color* c1    = new cydui::layout::color::Color("#000000");
};

typedef std::function<void()> ButtonAction;
#define action [state, this]

COMPONENT(Button)
  PROPS({
    std::string  text;
    ButtonAction on_action;
  })
  
  INIT(Button) }
  
  REDRAW(ev) {
    WITH_STATE(Button)
    
    ADD_TO(this, ({
      new primitives::Rectangle(
        state->hovering? state->c : state->c_dim, 0, 0,
        state->geom.content_w().val(), state->geom.content_h().val(),
        true
      ),
        (new primitives::Text(state->hovering? state->c1 : state->c, &state->font, 0, 0, props.text))
          ->set_margin(5, 5, 5, 5),
    }))
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
    props.on_action();
  }
};

#endif //CYD_UI_BUTTON_HPP
