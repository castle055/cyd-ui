//
// Created by castle on 9/15/22.
//

#ifndef CYD_UI_RUN_COMMAND_TASK_H
#define CYD_UI_RUN_COMMAND_TASK_H

#include <string>
#include <unistd.h>

class RunCommandTask {
  float progress = 0.0f;
  bool  running  = false;
  bool  complete = false;
  bool  error    = false;
public:
  
  void run(const char* cmd) {
    running = true;
    if (fork() == 0) {
      setsid();
      std::string c;
      c.append("");
      c.append(cmd);
      c.append(" & disown");
      fprintf(stdout, "running >>%s\n\r", c.c_str());
      std::system(c.c_str());
      
      exit(0);
    }
  }
};

#endif //CYD_UI_RUN_COMMAND_TASK_H
