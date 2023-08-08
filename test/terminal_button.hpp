//
// Created by castle on 9/11/22.
//

#ifndef CYD_UI_TERMINAL_BUTTON_HPP
#define CYD_UI_TERMINAL_BUTTON_HPP

#include <string>
#include "cydui.hpp"
#include "open_terminal_task.hpp"

STATE(TerminalButton) {
  OpenTerminalTask open_terminal_task;
  
  INIT_STATE(TerminalButton) {
  
  }
};

COMPONENT(TerminalButton) {
  PROPS({
    std::string cmd;
    bool hold_after_finish = false;
    bool scratch = true;
    
    int width = 85;
    int lines = 20;
  })
  
  INIT(TerminalButton) {
  }
  
  REDRAW {
    int p = state->open_terminal_task.get_progress();
    state->open_terminal_task.res();
    add({
      COMP(Rectangle)({
        .props = {
          .color = "#FCAE1E"_color,
          .filled = false,
        },
        .w = 500,
        .h = 30,
      }),
      COMP(Rectangle)({
        .props = {
          .color = "#FCAE1E"_color,
          .filled = true,
        },
        .x = 2,
        .y = 2,
        .w = (500 - 4) * p / 100,
        .h = 30 - 4,
      }),
    });
  }
  
  ON_CLICK(x, y, button) {
    //if (!state->open_terminal_task.is_running()) {
    state->open_terminal_task.run({
      this->props.width,
      this->props.lines,
      this->props.cmd.c_str()
    });
    //}
  }
};

#endif //CYD_UI_TERMINAL_BUTTON_HPP
