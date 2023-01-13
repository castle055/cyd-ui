//
// Created by castle on 1/10/23.
//

#ifndef CYD_UI_SYSMON_HPP
#define CYD_UI_SYSMON_HPP

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

STATE(SysMon)
  cydui::layout::color::Color* c_fg = new cydui::layout::color::Color("#FCAE1E");
  int value = 100;
  
  INIT_STATE(SysMon) {
  }
};

COMPONENT(SysMon)
  NO_PROPS INIT(SysMon) }
  
  REDRAW {
    state->value--;
    if (state->value == 0)
      state->value += 100;
    
    using namespace primitives;
    add({
      COMP(FlexBox)({
        .inner = {
          COMP(RadialIndicator)({
            .props = {
              .value = state->value,
              .show_value = true,
            },
          }),
          COMP(RadialIndicator)({
            .props = {
              .value = state->value,// - 23,
              .show_value = true,
            },
          }),
          COMP(RadialIndicator)({
            .props = {
              .value = state->value,// - 95,
              .show_value = true,
            },
          }),
        },
        .init = [this](FlexBox* f) {
          f->set_width(this->state->geom.content_w().val());
        },
        // TODO - FIXME - THIS DOES NOT WORK BECAUSE IT NEEDS THE ADD CALL TO HAVE BEEN EXECUTED FIRST
        //.w = this->state->geom.content_w().val(),
      }),
      COMP(FlexBox)({
        .props = {
          .vertical = true,
        },
        .inner = {
          COMP(BarIndicator)({
            .props = {
              .value = state->value,
            },
            .init = [](BarIndicator* bi) {
              bi->set_size(125, 8);
            },
          }),
          COMP(BarIndicator)({
            .props = {
              .value = state->value,
            },
            .init = [](BarIndicator* bi) {
              bi->set_size(125, 8);
            },
          }),
          COMP(BarIndicator)({
            .props = {
              .value = state->value,
            },
            .init = [](BarIndicator* bi) {
              bi->set_size(125, 8);
            },
          }),
          // TODO - write TimelineComponents and put one here
          COMP(BarIndicator)({
            .props = {
              .value = state->value,
            },
            .init = [](BarIndicator* bi) {
              bi->set_size(125, 12);
            },
          }),
        },
        .init = [this](FlexBox* f) {
          f->set_pos(this, 0, 50);
          f->set_height(100);
        },
      }),
    });
    set_padding(10, 10, 14, 10);
    set_border_enable(true);
  }
  
};

#endif //CYD_UI_SYSMON_HPP
