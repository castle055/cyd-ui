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
  
  void run(int w, int h, const char* cmd) {
    running = true;
    if (fork() == 0) {
      setsid();
      std::string c;
      c.append("st -c scratch -g");
      c.append(std::to_string(w));
      c.append("x");
      c.append(std::to_string(h));
      c.append("+0+0 -e sh -c '");
      c.append(cmd);
      c.append("' & disown");
      fprintf(stdout, "running %s", c.c_str());
      std::system(c.c_str());
      
      exit(0);
    }
  }
};

#endif //CYD_UI_OPEN_TERMINAL_TASK_HPP
