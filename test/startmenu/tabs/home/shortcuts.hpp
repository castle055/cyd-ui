//
// Created by castle on 1/6/23.
//

#ifndef CYD_UI_SHORTCUTS_HPP
#define CYD_UI_SHORTCUTS_HPP

#include "../../../../include/cydui.hpp"
#include "../../../components/button.hpp"
#include "../../../components/flexbox.hpp"
#include "../../../components/clock_module.hpp"
#include "../listview.hpp"
#include "../appbutton.hpp"
#include "../../config/startappscm.hpp"
#include <map>
#include <utility>

STATE(ShortCuts)
  cydui::layout::color::Color* c_fg = new cydui::layout::color::Color("#2d2310");
  int scroll = 0;
  
  std::vector<Application> apps = {
    {
      .name = "APP1"
    },
    {
      .name = "APP2",
      .terminal = true,
    },
    {
      .name = "APP3"
    },
    {
      .name = "APP3"
    },
    {
      .name = "APP3"
    },
    {
      .name = "APP3"
    },
    {
      .name = "APP3"
    },
    {
      .name = "APP3"
    },
  };
  
  INIT_STATE(ShortCuts) {
  }
};

COMPONENT(ShortCuts)
  NO_PROPS
  
  INIT(ShortCuts)
    DISABLE_LOG
  }
  
  REDRAW {
    WITH_STATE(ShortCuts)
    
    using namespace primitives;
    add({
      COMP(ListView)({
        .props = {
          .scroll = &state->scroll,
          .spacing = 12,
        },
        .inner = {
          FOR_EACH(AppButton)(state->apps,
            [](auto app) {
              return c_init_t<AppButton> {
                .props = {
                  .app = app,
                  .show_text = false,
                }};
            }),
        }
      }),
    });
  }
  
};

#endif //CYD_UI_SHORTCUTS_HPP
