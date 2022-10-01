//
// Created by castle on 9/11/22.
//

#ifndef CYD_UI_OPEN_TERMINAL_TASK_HPP
#define CYD_UI_OPEN_TERMINAL_TASK_HPP

#include <string>
#include <unistd.h>

class OpenTerminalTask {
  float progress = 0.0f;
  bool  running  = false;
  bool  complete = false;
  bool  error    = false;
public:
  
  void run(const char* cmd) {
    running = true;
    if (fork() == 0) {
      setsid();
      fprintf(stdout, "running %s", cmd);
      std::system(cmd);
      
      exit(0);
    }
  }
};

#endif //CYD_UI_OPEN_TERMINAL_TASK_HPP
