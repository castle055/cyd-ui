//
// Created by castle on 8/30/22.
//

#ifndef CYD_UI_WIFI_HPP
#define CYD_UI_WIFI_HPP

#include "../../include/cydui.hpp"
#include "../../src/logging/logging.hpp"
#include "banner.hpp"
#include "../components/button.hpp"
#include "../components/flexbox.hpp"
#include "../../src/events/properties/properties.hpp"

using namespace primitives;

static const logging::logger log = {.name = "MAIN", .on = true};

STATE(Test)
  IntProperty button {1};
  
  FlexBoxState* main_fbox = nullptr;
  
  cydui::layout::color::Color* c = new cydui::layout::color::Color("#FCAE1E");
  
  INIT_STATE(Test) {
    button.bind(this);
  }
};

COMPONENT(Test)
  NO_PROPS
  
  INIT(Test) {
  
  }
  
  REDRAW(ev) {
    WITH_STATE(Test)
    
    ADD_TO(this, {
      N(FlexBox, ({ .vertical = false }), ({
        N(FlexBox, ({ .vertical = true }), ({
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
            rectangle(state->c, 32, 32, true),
            rectangle(state->c, 32, 32, true),
            rectangle(state->c, 32, 32, true),
        }), {
          thisFlexBox->set_border_enable(true);
          if (state->main_fbox) {
            thisFlexBox->set_height(state->main_fbox->geom.content_h());
          }
        }),
          rectangle(state->c, 32, 32, true),
          rectangle(state->c, 32, 32, true),
          rectangle(state->c, 32, 32, true),
          rectangle(state->c, 32, 32, true),
      }), {
        state->main_fbox = (FlexBoxState*)thisFlexBox->state;
        thisFlexBox->set_padding(5, 5, 5, 5);
        thisFlexBox->set_margin(5, 5, 5, 5);
        thisFlexBox->set_border_enable(true);
        thisFlexBox->set_size(state->geom.content_w(), state->geom.content_h());
      })
    })
  }
};

#endif //CYD_UI_WIFI_HPP
