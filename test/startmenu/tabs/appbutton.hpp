//
// Created by castle on 1/4/23.
//

#ifndef CYD_UI_APPBUTTON_HPP
#define CYD_UI_APPBUTTON_HPP

#include <functional>
#include "../../../include/cydui.hpp"
#include "../../../include/logging.hpp"
#include "../../tasks/list_apps_task.hpp"

STATE(AppButton)
  bool hovering = false;
  
  cydui::layout::fonts::Font font {
    .name = "Fira Code Retina",
    .size = 10
  };
  
  cydui::layout::color::Color* c      = new cydui::layout::color::Color("#FCAE1E");
  cydui::layout::color::Color* c_term = new cydui::layout::color::Color("#00cd73");
  cydui::layout::color::Color* c_dim  = new cydui::layout::color::Color("#2d2310");
  cydui::layout::color::Color* c1     = new cydui::layout::color::Color("#000000");
  
};

typedef std::function<void(int)> AppButtonAction;

COMPONENT(AppButton)
  PROPS({
    Application app;
    cydui::layout::fonts::Font * font  = nullptr;
    cydui::layout::color::Color* c     = nullptr;
    cydui::layout::color::Color* c_dim = nullptr;
    cydui::layout::color::Color* c1    = nullptr;
    
    int x = 0;
    int y = 0;
    
    AppButtonAction on_action = [](int) { };
  })
  
  INIT(AppButton)
    if (!this->props.font) this->props.font   = &state->font;
    if (!this->props.c) this->props.c         = state->c;
    if (!this->props.c_dim) this->props.c_dim = state->c_dim;
    if (!this->props.c1) this->props.c1       = state->c1;
    
    if (this->props.app.terminal) {
      this->props.c = state->c_term;
    }
  }
  
  REDRAW {
    WITH_STATE(AppButton)
    
    using namespace primitives;
    add({
      COMP(Rectangle)({
        .props = {
          .color = state->hovering? props.c : props.c_dim,
          .filled = true
        },
        .init = [this, state](Rectangle* r) {
          r->set_pos(this, 0, 0);
          r->set_size(state->geom.content_w(), state->geom.content_h());
        },
      }),
      COMP(Rectangle)({
        .props = {
          .color = state->hovering? props.c1 : props.c,
          .filled = true
        },
        .init = [this](Rectangle* r) {
          r->set_pos(this, 5, 5);
          r->set_size(24, 24);
        },
      }),
      COMP(Text)({
        .props = {
          .color = state->hovering? props.c1 : props.c,
          .font = props.font,
          .text = props.app.name,
        },
        .init = [this, state](Text* t) {
          t->set_pos(this, 34, 7);
          t->set_margin(5, 5, 5, 5);
          t->set_width(state->geom.custom_width? state->geom.content_w() : 125);
        },
      })
    });
    
  }
  
  void on_mouse_enter(int x, int y) override {
    auto state = (AppButtonState*)this->state;
    state->hovering = true;
    state->dirty();
  }
  
  void on_mouse_exit(int x, int y) override {
    auto state = (AppButtonState*)this->state;
    state->hovering = false;
    state->dirty();
  }
  
  void on_mouse_click(int x, int y, int button) override {
    auto state = (AppButtonState*)this->state;
    if (state->hovering) {
      state->dirty();
      props.on_action(button);
      state->hovering = false;
    }
  }
};

#endif //CYD_UI_APPBUTTON_HPP
