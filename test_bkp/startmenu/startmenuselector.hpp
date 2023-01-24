//
// Created by castle on 1/4/23.
//

#ifndef CYD_UI_STARTMENUSELECTOR_HPP
#define CYD_UI_STARTMENUSELECTOR_HPP

#include "../../include/cydui.hpp"
#include "../components/clock_module.hpp"
#include "../components/flexbox.hpp"

STATE(StartMenuSelector) {
  cydui::layout::fonts::Font font {
    .name = "Fira Code Retina",
    .size = 10
  };
  
  INIT_STATE(StartMenuSelector) {
  }
};

typedef std::function<void(int)> SelectorAction;

COMPONENT(StartMenuSelector) {
  PROPS({
    std::string    text;
    SelectorAction on_action = [](int i) { };
  })
  
  INIT(StartMenuSelector) {
  }
  
  REDRAW {
    add({
      N(FlexBox, ({ .vertical = true }), ({
        N(Button, ({
          .text = "HOM",
          .font = &state->font,
          .on_action = action {props.on_action(0);},
        }), ({ }), {
          thisButton->set_width(50);
          thisButton->set_height(50);
        }),
          N(Button, ({
            .text = "APP",
            .font = &state->font,
            .on_action = action {props.on_action(1);},
          }), ({ }), {
            thisButton->set_width(50);
            thisButton->set_height(50);
          }),
          N(Button, ({
            .text = "SCH",
            .font = &state->font,
            .on_action = action {props.on_action(2);},
          }), ({ }), {
            thisButton->set_width(50);
            thisButton->set_height(50);
          }),
      }), {
        thisFlexBox->set_width(50);
        thisFlexBox->set_height(150);
        thisFlexBox->set_pos(this, 0, 0);
      })
    });
  }
};

#endif //CYD_UI_STARTMENUSELECTOR_HPP
