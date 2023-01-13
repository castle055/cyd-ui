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
  
  StartAppsCM* config = new StartAppsCM("/home/castle/.config/corium/startmenu/start.apps.yml");
  ListAppsTask list_apps_task;
  
  INIT_STATE(AppsTab) {
    list_apps_task.run({config});
  }
};

COMPONENT(AppsTab)
  NO_PROPS INIT(AppsTab) }
  REDRAW {
    add({
      COMP(Rectangle)({
        .props = {
          .color = &c::Black,
          .filled = true,
        },
        .init = [this](Rectangle* r) {
          r->set_pos(this, 0, 0);
          r->set_size(state->geom.abs_w().val(), state->geom.abs_h().val());
        },
      }),
      state->list_apps_task.is_complete()?
      COMP(ListView)({
        .props = {
          .scroll = &state->scroll,
        },
        .inner = {
          FOR_EACH(AppSection)(state->list_apps_task.res()->section_apps,
            [](auto section_data) {
              return c_init_t<AppSection> {
                .props = {
                  .letter = section_data.first,
                  .apps = section_data.second,
                }};
            }),
          FOR_EACH(AppSection)(state->list_apps_task.res()->apps,
            [](auto section_data) {
              return c_init_t<AppSection> {
                .props = {
                  .letter = section_data.first,
                  .apps = section_data.second,
                }};
            }),
        }
      }) : NULLCOMP,
    });
  }
  
  void on_scroll(int d) override {
    state->scroll += d;
    if (state->scroll > 0) state->scroll = 0;
    state->dirty();
  }
};

#endif //CYD_UI_APPSTAB_HPP
