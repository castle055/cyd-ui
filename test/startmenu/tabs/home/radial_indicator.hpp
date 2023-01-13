//
// Created by castle on 1/10/23.
//

#ifndef CYD_UI_RADIAL_INDICATOR_HPP
#define CYD_UI_RADIAL_INDICATOR_HPP

#include "../../../../include/cydui.hpp"

STATE(RadialIndicator)
  cydui::layout::color::Color* color = new cydui::layout::color::Color("#FCAE1E");
  
  cydui::layout::fonts::Font font_smaller {
    .name = "Fira Code Retina",
    .size = 6,
  };
  cydui::layout::fonts::Font font {
    .name = "Fira Code Retina",
    .size = 7,
  };
  
  INIT_STATE(RadialIndicator) {
  }
};

COMPONENT(RadialIndicator)
  PROPS({
    int  value      = 50;
    bool show_value = false;
    cydui::layout::color::Color* color = nullptr;
  })
  
  INIT(RadialIndicator)
    DISABLE_LOG
    if (!this->props.color) this->props.color = state->color;
  }
  
  REDRAW {
    WITH_STATE(RadialIndicator)
    
    using namespace primitives;
    add({
      COMP(Arc)({
        .props = {
          .color = state->color,
          .filled = true,
          .a0 = 270,
          .a1 = -((props.value * 360) / 100),
        },
        .init = [this, state](Arc* a) {
          a->set_pos(this, 0, 0);
          a->set_size(30, 30);
        },
      }),
      COMP(Arc)({
        .props = {
          .color = &c::Black,
          .filled = true,
          .a0 = 270,
          .a1 = -((props.value * 360) / 100),
        },
        .init = [this, state](Arc* a) {
          a->set_pos(this, 3, 3);
          a->set_size(24, 24);
        },
      }),
      COMP(Arc)({
        .props = {
          .color = state->color,
          .filled = true,
          .a0 = 270,
          .a1 = -((props.value * 360) / 100),
        },
        .init = [this, state](Arc* a) {
          a->set_pos(this, 4, 4);
          a->set_size(22, 22);
        },
      }),
      COMP(Arc)({
        .props = {
          .color = &c::Black,
          .filled = true,
          .a0 = 270,
          .a1 = -((props.value * 360) / 100),
        },
        .init = [this, state](Arc* a) {
          a->set_pos(this, 7, 7);
          a->set_size(16, 16);
        },
      }),
      props.show_value?
      COMP(Text)({
        .props = {
          .color = state->color,
          .font = props.value == 100? &state->font_smaller : &state->font,
          .text = std::to_string(props.value),
        },
        .init = [this, state](Text* t) {
          if (props.value == 100) {
            t->set_pos(this, 8, 12);
          } else if (props.value < 10) {
            t->set_pos(this, 11, 12);
          } else {
            t->set_pos(this, 10, 12);
          }
          t->set_width(10);
        },
      }) : NULLCOMP,
    });
  }
  
};

#endif //CYD_UI_RADIAL_INDICATOR_HPP
