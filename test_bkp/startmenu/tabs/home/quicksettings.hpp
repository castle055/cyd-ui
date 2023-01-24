//
// Created by castle on 1/11/23.
//

#ifndef CYD_UI_QUICKSETTINGS_HPP
#define CYD_UI_QUICKSETTINGS_HPP

#include "../../../../include/cydui.hpp"
#include "../../../components/button.hpp"
#include "../../../components/flexbox.hpp"
#include "../../config/startappscm.hpp"
#include <utility>

STATE(QuickSettings)
  cydui::layout::color::Color* c_fg = new cydui::layout::color::Color("#2d2310");
  
  std::vector<Application> qkset1 = {
    {.name = "WFI"},
    {.name = "BTH",},
    {.name = "ST1"},
    {.name = "ST2"},
    {.name = "ST3"},
    {.name = "ST4"},
  };
  std::vector<Application> qkset2 = {
    {.name = "ST2"},
    {.name = "ST3"},
    {.name = "ST4"},
    {.name = "ST2"},
    {.name = "ST3"},
    {.name = "ST4"},
  };
  
  INIT_STATE(QuickSettings) {
  }
};

static const int button_size = 45;
static const int spacing     = 4;

COMPONENT(QuickSettings)
  NO_PROPS INIT(QuickSettings) }
  REDRAW {
    add({
      COMP(FlexBox)({
        .props = {
          .vertical = false,
        },
        .inner = {
          FOR_EACH(Button)(state->qkset2,
            [](auto app) {
              return c_init_t<Button> {
                .props = {
                  .text = app.name,
                },
                .init = [](Button* b) {
                  b->set_width(button_size);
                  b->set_height(button_size);
                },
              };
            }),
        },
        .init = [this](FlexBox* f) {
          f->set_pos(this, 0, button_size + spacing);
          f->set_width(state->qkset2.size() * (button_size + spacing));
        },
      }),
      COMP(FlexBox)({
        .props = {
          .vertical = false,
        },
        .inner = {
          FOR_EACH(Button)(state->qkset1,
            [](auto app) {
              return c_init_t<Button> {
                .props = {
                  .text = app.name,
                },
                .init = [](Button* b) {
                  b->set_width(button_size);
                  b->set_height(button_size);
                },
              };
            }),
        },
        .init = [this](FlexBox* f) {
          f->set_width(state->qkset1.size() * (button_size + spacing));
        },
      }),
    });
  }
  
};

#endif //CYD_UI_QUICKSETTINGS_HPP
