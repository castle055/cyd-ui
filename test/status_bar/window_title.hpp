//
// Created by castle on 10/2/22.
//

#ifndef CYD_UI_WINDOW_TITLE_HPP
#define CYD_UI_WINDOW_TITLE_HPP

#include "../../include/cydui.hpp"

using namespace primitives;

STATE(WinTitle)
  
  cydui::layout::fonts::Font font {
    .name = "Fira Code Retina",
    .size = 10
  };
  
  cydui::layout::color::Color* color = new cydui::layout::color::Color("#FCAE1E");
  
};

COMPONENT(WinTitle)
  NO_PROPS
  
  INIT(WinTitle) DISABLE_LOG
  
  }
  
  REDRAW {
    WITH_STATE(WinTitle)
    
    ADD_TO(this, {
      N(Text, ({
        .color = state->color,
        .font = &state->font,
        .text = "window_title"
      }), ({ }), {
        thisText->set_pos(this, 20, 5);
      })
    })
  }
};

#endif //CYD_UI_WINDOW_TITLE_HPP
