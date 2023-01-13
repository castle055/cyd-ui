//
// Created by castle on 9/15/22.
//

#ifndef CYD_UI_RUN_COMMAND_TASK_H
#define CYD_UI_RUN_COMMAND_TASK_H

#include <string>
#include <unistd.h>

SIMPLE_TASK(RunCommandTask, {
  const char* cmd;
}, {
  if (fork() == 0) {
    if (fork() == 0) {
      setsid();
      std::string c;
      c.append("");
      c.append(this->args.cmd);
      c.append(" >/dev/null 2>&1 & disown");
      fprintf(stdout, "running >>%s\n\r", c.c_str());
      std::system(c.c_str());
      
      exit(0);
    }
  }
})

#endif //CYD_UI_RUN_COMMAND_TASK_H
