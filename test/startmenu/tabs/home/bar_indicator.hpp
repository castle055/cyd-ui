//
// Created by castle on 1/10/23.
//

#ifndef CYD_UI_BAR_INDICATOR_HPP
#define CYD_UI_BAR_INDICATOR_HPP

#include "../../../../include/cydui.hpp"

STATE(BarIndicator)
  cydui::layout::color::Color* color = new cydui::layout::color::Color("#FCAE1E");
  
  INIT_STATE(BarIndicator) {
  }
};

COMPONENT(BarIndicator)
  PROPS({
    int value = 50;
    cydui::layout::color::Color* color = nullptr;
  })
  
  INIT(BarIndicator)
    DISABLE_LOG
    if (!this->props.color) this->props.color = state->color;
  }
  
  REDRAW {
    WITH_STATE(BarIndicator)
    
    using namespace primitives;
    add({
      COMP(Rectangle)({
        .props = {
          .color = state->color,
          .filled = false,
        },
        .init = [this, state](Rectangle* r) {
          r->set_pos(this, 0, 0);
          r->set_size(state->geom.content_w().val(), std::max(state->geom.content_h().val(), 8));
        },
      }),
      COMP(Rectangle)({
        .props = {
          .color = state->color,
          .filled = true,
        },
        .init = [this, state](Rectangle* r) {
          r->set_pos(this, 2, 3);
          r->set_size((props.value * (state->geom.content_w().val() - 3)) / 100,
            std::max(state->geom.content_h().val(), 8) - 5);
        },
      }),
    });
  }
};

#endif //CYD_UI_BAR_INDICATOR_HPP
