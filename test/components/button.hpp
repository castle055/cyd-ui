//
// Created by castle on 8/31/22.
//

#ifndef CYD_UI_BUTTON_HPP
#define CYD_UI_BUTTON_HPP

#include <functional>
#include "../../include/cydui.hpp"
#include "../../include/logging.hpp"

STATE(Button)
  bool hovering = false;
  
  cydui::layout::fonts::Font font {
    .name = "Fira Code Retina",
    .size = 10
  };
  
  cydui::layout::color::Color* c     = new cydui::layout::color::Color("#FCAE1E");
  cydui::layout::color::Color* c_dim = new cydui::layout::color::Color("#2d2310");
  cydui::layout::color::Color* c1    = new cydui::layout::color::Color("#000000");
};

typedef std::function<void()> ButtonAction;
#define action [&,state, this]

COMPONENT(Button)
  PROPS({
    std::string text = "Text";
    cydui::layout::fonts::Font* font;
    ButtonAction on_action = []() { };
    cydui::layout::color::Color* c     = nullptr;
    cydui::layout::color::Color* c_dim = nullptr;
    cydui::layout::color::Color* c1    = nullptr;
  })
  
  INIT(Button)
    if (!this->props.font) this->props.font   = &state->font;
    if (!this->props.c) this->props.c         = state->c;
    if (!this->props.c_dim) this->props.c_dim = state->c_dim;
    if (!this->props.c1) this->props.c1       = state->c1;
  }
  
  REDRAW(ev) {
    WITH_STATE(Button)
    
    using namespace primitives;
    ADD_TO(this, ({
      N(Rectangle, ({
        .color = state->hovering? props.c : props.c_dim,
        .filled = true
      }), ({ }), {
        thisRectangle->set_pos(this, 0, 0);
        thisRectangle->set_size(state->geom.content_w(), state->geom.content_h());
      }),
        N(Text, ({
          .color = state->hovering? props.c1 : props.c,
          .font = props.font,
          .text = props.text,
        }), ({ }), {
          thisText->set_margin(5, 5, 5, 5);
          thisText->set_width(state->geom.custom_width? state->geom.content_w() : 125);
        })
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
