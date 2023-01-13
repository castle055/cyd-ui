//
// Created by castle on 1/10/23.
//

#ifndef CYD_UI_BAR_INDICATOR_HPP
#define CYD_UI_BAR_INDICATOR_HPP

#include "../../../../include/cydui.hpp"

STATE(BarIndicator)
  cydui::layout::color::Color* color = new cydui::layout::color::Color("#FCAE1E");
  
  cydui::layout::fonts::Font font {
    .name = "Fira Code Retina",
    .size = 6,
  };
  
  INIT_STATE(BarIndicator) {
  }
};

COMPONENT(BarIndicator)
  PROPS({
    std::string text       = "";
    int         value      = 50;
    bool        show_value = false;
    cydui::layout::color::Color* color = nullptr;
  })
  
  INIT(BarIndicator)
    DISABLE_LOG
    if (!this->props.color) this->props.color = state->color;
  }
  
  REDRAW {
    int y = props.text.empty()? 0 : 9;
    
    using namespace primitives;
    add({
      !props.text.empty()?
      COMP(Text)({
        .props = {
          .color = state->color,
          .font = &state->font,
          .text = props.text,
        },
        .init = [this](Text* a) {
          a->set_pos(this, 3, 0);
          a->set_size(30, 8);
        },
      }) : NULLCOMP,
      props.show_value?
      COMP(Text)({
        .props = {
          .color = state->color,
          .font = &state->font,
          .text = std::to_string(props.value).append(" %"),
        },
        .init = [this](Text* a) {
          a->set_pos(this, state->geom.content_w().val() - 30, 0);
          a->set_size(30, 8);
        },
      }) : NULLCOMP,
      COMP(Rectangle)({
        .props = {
          .color = state->color,
          .filled = false,
        },
        .init = [this, y](Rectangle* r) {
          r->set_pos(this, 0, y + 0);
          r->set_size(state->geom.content_w().val(), std::max(state->geom.content_h().val(), 8));
        },
      }),
      COMP(Rectangle)({
        .props = {
          .color = state->color,
          .filled = true,
        },
        .init = [this, y](Rectangle* r) {
          r->set_pos(this, 2, y + 3);
          r->set_size((props.value * (state->geom.content_w().val() - 3)) / 100,
            std::max(state->geom.content_h().val(), 8) - 5);
        },
      }),
    });
  }
};

#endif //CYD_UI_BAR_INDICATOR_HPP
