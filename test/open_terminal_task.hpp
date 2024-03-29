//
// Created by castle on 9/11/22.
//

#ifndef CYD_UI_OPEN_TERMINAL_TASK_HPP
#define CYD_UI_OPEN_TERMINAL_TASK_HPP

#include "cydui.hpp"
#include <string>
#include <unistd.h>

SIMPLE_TASK(OpenTerminalTask, {
  int w = 110;
  int h = 30;
  std::string cmd;
}, {
  using namespace std::chrono_literals;
  
  float p = this->get_progress();
  while (p < 100.0) {
    this->set_progress(p + .3);
    std::this_thread::sleep_for(10ms);
    p += .3;
  }
  //if (fork() == 0) {
  //  setsid();
  //  std::string c;
  //  c.append("st -c scratch -g");
  //  c.append(std::to_string(args.w));
  //  c.append("x");
  //  c.append(std::to_string(args.h));
  //  c.append("+0+0 -e sh -c '");
  //  c.append(args.cmd.c_str());
  //  c.append("' & disown");
  //  log.debug("running %s", c.c_str());
  //  std::system(c.c_str());
  //  
  //  exit(0);
  //}
})

#endif //CYD_UI_OPEN_TERMINAL_TASK_HPP
