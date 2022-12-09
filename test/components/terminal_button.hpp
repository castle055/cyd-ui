//
// Created by castle on 9/11/22.
//

#ifndef CYD_UI_TERMINAL_BUTTON_HPP
#define CYD_UI_TERMINAL_BUTTON_HPP

#include "../../include/cydui.hpp"
#include "button.hpp"
#include "../tasks/open_terminal_task.hpp"

STATE(TerminalButton)
  OpenTerminalTask open_terminal_task;
  
  INIT_STATE(TerminalButton) {
  
  }
};

COMPONENT(TerminalButton)
  PROPS({
    std::string cmd;
    bool        hold_after_finish = false;
    bool        scratch           = true;
    
    int width = 85;
    int lines = 20;
  })
  
  std::string full_cmd;
  
  INIT(TerminalButton)
    full_cmd = get_full_cmd();
  }
  
  REDRAW {
    WITH_STATE(TerminalButton)
    
    ADD_TO(this, ({
      N(Button, ({
        .text = "T",
        .on_action = action {
          state->open_terminal_task.run(full_cmd.c_str());
        }
      })),
    }))
  }

private:
  
  std::string get_full_cmd() const {
    std::string cmd;
    cmd.append("st");
    if (props.scratch) {
      cmd.append(" -c scratch");
    }
    cmd.append(" -g");
    cmd.append(std::to_string(props.width));
    cmd.append("x");
    cmd.append(std::to_string(props.lines));
    cmd.append("+0+0");
    
    cmd.append(" -e sh -c '");
    cmd.append(props.cmd);
    
    if (props.hold_after_finish) {
      cmd.append(" ; while 1; do sleep 100; done");
    }
    
    cmd.append("'");
    
    return cmd;
  }
};

#endif //CYD_UI_TERMINAL_BUTTON_HPP
