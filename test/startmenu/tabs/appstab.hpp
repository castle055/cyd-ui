//
// Created by castle on 1/4/23.
//

#ifndef CYD_UI_APPSTAB_HPP
#define CYD_UI_APPSTAB_HPP

#include "../../../include/cydui.hpp"
#include "../../components/button.hpp"
#include "../../components/flexbox.hpp"
#include "../../components/clock_module.hpp"
#include "appbutton.hpp"
#include "appsection.hpp"
#include "listview.hpp"
#include "../../tasks/list_apps_task.hpp"
#include <map>
#include <utility>

STATE(AppsTab)
  cydui::layout::color::Color* c_fg = new cydui::layout::color::Color("#2d2310");
  int scroll = 0;
  
  StartAppsCM  config = StartAppsCM("/home/castle/.config/corium/startmenu/start.apps.yml");
  ListAppsTask list_apps_task;
  
  INIT_STATE(AppsTab) {
    list_apps_task.run(config);
  }
};

COMPONENT(AppsTab)
  NO_PROPS
  
  INIT(AppsTab)
    DISABLE_LOG
  }
  
  REDRAW {
    WITH_STATE(AppsTab)
    
    using namespace primitives;
    add({
      COMP(ListView)({
        .props = {
          .scroll = state->scroll,
        },
        .inner = {
          FOR_EACH(AppSection)(state->list_apps_task.section_apps,
            [](auto section_data) {
              return c_init_t<AppSection> {
                .props = {
                  .letter = section_data.first,
                  .apps = section_data.second,
                }};
            }),
          FOR_EACH(AppSection)(state->list_apps_task.apps,
            [](auto section_data) {
              return c_init_t<AppSection> {
                .props = {
                  .letter = section_data.first,
                  .apps = section_data.second,
                }};
            }),
        }
      }),
    });
  }
  
  void on_scroll(int d) override {
    auto* state = (AppsTabState*)this->state;
    state->scroll += d;
    if (state->scroll > 0) state->scroll = 0;
    state->dirty();
  }
};

#endif //CYD_UI_APPSTAB_HPP
