//
// Created by castle on 1/4/23.
//

#ifndef CYD_UI_POWERSELECTOR_HPP
#define CYD_UI_POWERSELECTOR_HPP

#include "../../include/cydui.hpp"
#include "../components/clock_module.hpp"
#include "../components/flexbox.hpp"
#include "../../src/graphics/events.hpp"

STATE(PowerSelector)
  cydui::layout::color::Color* c_reb = new cydui::layout::color::Color("#00F00B");
  cydui::layout::color::Color* c_sdn = new cydui::layout::color::Color("#EC0010");
  cydui::layout::fonts::Font font {
    .name = "Fira Code Retina",
    .size = 10
  };
  
  bool toggled = false;
  
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
    
    add({
      COMP(FlexBox)({
        .props = {.vertical = true},
        .inner = {
          state->toggled?
          COMP(Button)({
            .props = {
              .text = "REB",
              .font = &state->font,
              .on_action = action {
                state->toggled = !state->toggled;
                state->dirty();
              },
              .c = state->c_reb,
            },
            .init = [](Button* b) {
              b->set_width(50);
              b->set_height(30);
            },
          }) : nullptr,
          state->toggled?
          COMP(Button)({
            .props = {
              .text = "OFF",
              .font = &state->font,
              .on_action = action {
                state->toggled = !state->toggled;
                state->dirty();
              },
              .c = state->c_sdn,
            },
            .init = [](Button* b) {
              b->set_width(50);
              b->set_height(30);
            },
          }) : nullptr,
          state->toggled?
          COMP(Button)({
            .props = {
              .text = "HIB",
              .font = &state->font,
              .on_action = action {
                state->toggled = !state->toggled;
                state->dirty();
              },
            },
            .init = [](Button* b) {
              b->set_width(50);
              b->set_height(30);
            },
          }) : nullptr,
          state->toggled?
          COMP(Button)({
            .props = {
              .text = "SUS",
              .font = &state->font,
              .on_action = action {
                state->toggled = !state->toggled;
                state->dirty();
              },
            },
            .init = [](Button* b) {
              b->set_width(50);
              b->set_height(30);
            },
          }) : nullptr,
          state->toggled?
          COMP(Button)({
            .props = {
              .text = "LCK",
              .font = &state->font,
              .on_action = action {
                state->toggled = !state->toggled;
                state->dirty();
              },
            },
            .init = [](Button* b) {
              b->set_width(50);
              b->set_height(30);
            },
          }) : nullptr,
          COMP(Button)({
            .props = {
              .text = "PWR",
              .font = &state->font,
              .on_action = action {
                state->toggled = !state->toggled;
                state->dirty();
                events::emit<RedrawEvent>({ });
              },
            },
            .init = [](Button* b) {
              b->set_width(50);
              b->set_height(50);
            },
          }),
        },
        .init = [this, state](FlexBox* f) {
          f->set_width(50);
          if (state->toggled) {
            f->set_height(231);
          } else {
            f->set_height(56);
          }
          f->set_pos(this, 0, 0);
        },
      }),
    });
  }
};

#endif //CYD_UI_POWERSELECTOR_HPP
