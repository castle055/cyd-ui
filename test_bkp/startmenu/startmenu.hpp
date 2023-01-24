//
// Created by castle on 1/4/23.
//

#ifndef CYD_UI_STARTMENU_HPP
#define CYD_UI_STARTMENU_HPP

#include "../../include/cydui.hpp"
#include "../components/button.hpp"
#include "../components/flexbox.hpp"
#include "../components/clock_module.hpp"
#include "startmenuselector.hpp"
#include "powerselector.hpp"
#include "tabs/hometab.hpp"
#include "tabs/appstab.hpp"
#include "tabs/searchtab.hpp"

STATE(StartMenu) {
  enum StartMenuTabs {
    HOME,
    APPS,
    SEARCH,
  };
  
  StartMenuTabs selected_tab = HOME;
  cydui::layout::color::Color* c_fg = new cydui::layout::color::Color("#2d2310");
  
  INIT_STATE(StartMenu) {
  }
};

COMPONENT(StartMenu) {
  NO_PROPS
  
  INIT(StartMenu) {
  }
  
  REDRAW {
    add({
      COMP(FlexBox)({
        .props = {
          .vertical = true,
        },
        .inner = {
          COMP(StartMenuSelector)({
            .props = {
              .on_action = [this](int i) {
                log.debug("SELECTED= %d", i);
                log.debug("FROM= %d", state->selected_tab);
                switch (i) {
                  case 0: state->selected_tab = StartMenuState::HOME;
                    break;
                  case 1: state->selected_tab = StartMenuState::APPS;
                    break;
                  case 2: state->selected_tab = StartMenuState::SEARCH;
                    break;
                  default:break;
                }
                state->dirty();
                log.debug("TO= %d", state->selected_tab);
              }
            }
          }),
          COMP(PowerSelector)({ }),
        },
        .init = [this](auto* f) {
          f->set_width(54);
          f->set_height(800);
          f->set_pos(this, 0, 0);
        }
      }),
      COMP(Rectangle)({
        .props = {
          .color = state->c_fg,
          .filled = true
        },
        .init = [this](auto* r) {
          r->set_pos(this, 54, 0);
          r->set_size(3, state->geom.content_h());
        }
      }),
      state->selected_tab == StartMenuState::HOME?
      COMP(HomeTab)({
        .init = [this](auto* h) { h->set_pos(this, 60, 0); }
      }) :
      (state->selected_tab == StartMenuState::APPS?
       COMP(AppsTab)({
         .init = [this](auto* h) { h->set_pos(this, 60, 0); }
       }) :
       (state->selected_tab == StartMenuState::SEARCH?
        COMP(Rectangle)({
          .props = {
            .color = state->c_fg,
            .filled = true
          },
          .init = [this](auto* r) {
            r->set_pos(this, 200, 0);
            r->set_size(30, state->geom.content_h());
          }
        }) : NULLCOMP
       )
      )
    });
  }
};

#endif //CYD_UI_STARTMENU_HPP
