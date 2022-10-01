//
// Created by castle on 9/27/22.
//

#ifndef CYD_UI_WORKSPACES_HPP
#define CYD_UI_WORKSPACES_HPP

#include "../../include/cydui.hpp"
#include "../components/button.hpp"
#include "../components/flexbox.hpp"

STATE(Workspaces)
  
  INIT_STATE(Workspaces) {
  }
};

COMPONENT(Workspaces)
  NO_PROPS
  
  INIT(Workspaces)
    DISABLE_LOG
  }
  
  REDRAW(ev) {
    WITH_STATE(Workspaces)
    
    ADD_TO(this, {
      N(FlexBox, ({ .vertical = false }), ({
        N(Button, ({
          .text = "1",
          .on_action = action { }
        }), ({ }), {
          thisButton->set_width(24);
          thisButton->set_border_enable(true);
        }),
          N(Button, ({
            .text = "2",
            .on_action = action { }
          })),
          N(Button, ({
            .text = "3",
            .on_action = action { }
          })),
          N(Button, ({
            .text = "4",
            .on_action = action { }
          })),
          N(Button, ({
            .text = "5",
            .on_action = action { }
          })),
          N(Button, ({
            .text = "6",
            .on_action = action { }
          })),
          N(Button, ({
            .text = "7",
            .on_action = action { }
          })),
          N(Button, ({
            .text = "8",
            .on_action = action { }
          })),
          N(Button, ({
            .text = "9",
            .on_action = action { }
          })),
      }))
    })
  }
};

#endif//CYD_UI_WORKSPACES_HPP
