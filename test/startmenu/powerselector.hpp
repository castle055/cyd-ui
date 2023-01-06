//
// Created by castle on 1/4/23.
//

#ifndef CYD_UI_POWERSELECTOR_HPP
#define CYD_UI_POWERSELECTOR_HPP

#include "../../include/cydui.hpp"
#include "../components/clock_module.hpp"
#include "../components/flexbox.hpp"

STATE(PowerSelector)
  cydui::layout::fonts::Font font {
    .name = "Fira Code Retina",
    .size = 10
  };
  
  INIT_STATE(PowerSelector) {
  }
};

COMPONENT(PowerSelector)
  NO_PROPS
  
  INIT(PowerSelector)
    DISABLE_LOG
  }
  
  REDRAW {
    WITH_STATE(PowerSelector)
    
    ADD_TO(this, {
      N(FlexBox, ({ .vertical = true }), ({
        N(Button, ({
          .text = "P",
          .font = &state->font,
          .on_action = action { },
        }), ({ }), {
          thisButton->set_width(50);
          thisButton->set_height(50);
        }),
      }), {
        thisFlexBox->set_width(50);
        thisFlexBox->set_height(50);
        thisFlexBox->set_pos(this, 0, 0);
      })
    })
  }
};

#endif //CYD_UI_POWERSELECTOR_HPP
