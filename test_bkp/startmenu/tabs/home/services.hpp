//
// Created by castle on 1/13/23.
//

#ifndef CYD_UI_SERVICES_HPP
#define CYD_UI_SERVICES_HPP

#include "../../../../include/cydui.hpp"
#include "../../../components/button.hpp"
#include "../../../components/flexbox.hpp"
#include "../../../components/clock_module.hpp"
#include "../listview.hpp"
#include "../appbutton.hpp"
#include "../../config/startappscm.hpp"
#include "./radial_indicator.hpp"
#include "./bar_indicator.hpp"
#include <map>
#include <utility>


STATE(Services)
  cydui::layout::color::Color* c_fg = new cydui::layout::color::Color("#FCAE1E");
  INIT_STATE(Services) {
  
  }
};

COMPONENT(Services)
  NO_PROPS INIT(Services) }
  
  REDRAW {
    set_padding(10, 10, 14, 10);
    set_border_enable(true);
  }
};

#endif //CYD_UI_SERVICES_HPP
