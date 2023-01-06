//
// Created by castle on 8/30/22.
//

#ifndef CYD_UI_WIFI_HPP
#define CYD_UI_WIFI_HPP

#include "../../include/cydui.hpp"
#include "../../include/logging.hpp"
#include "banner.hpp"
#include "../components/button.hpp"
#include "../components/flexbox.hpp"
#include "../../include/properties.hpp"

using namespace primitives;


STATE(Test)
  IntProperty button {1};
  
  cydui::layout::color::Color* c = new cydui::layout::color::Color("#FCAE1E");
  
  INIT_STATE(Test) {
    button.bind(this);
  }
};

COMPONENT(Test)
  NO_PROPS
  
  INIT(Test)
    DISABLE_LOG
  }
  
  REDRAWw(Test, {
    N(FlexBox, ({ .vertical = false }), ({
      N(FlexBox, ({ .vertical = true }), ({
        N(Banner),
        N(Banner),
        N(Banner),
        N(Banner),
        N(Banner),
        N(Banner),
        (state->button.val() == 1)?
        N(Button, ({
          .text = "TEST 1",
          .on_action = action {
            state->button = 2;
            log.info("CLICK 1");
          }
        })) :
        N(Button, ({
          .text = "TEST 2",
          .on_action = action {
            state->button = 1;
            log.info("CLICK 2");
          }
        })),
      }), {
        thisFlexBox->set_border_enable(true);
        thisFlexBox->set_height(thisFlexBox->parent->state->geom.content_h());
      }),
    }), {
      thisFlexBox->set_border_enable(true);
      thisFlexBox->set_size(state->geom.content_w(), state->geom.content_h());
    })
  })
};

#endif //CYD_UI_WIFI_HPP
