//
// Created by castle on 9/15/22.
//

#ifndef CYD_UI_RUN_COMMAND_TASK_H
#define CYD_UI_RUN_COMMAND_TASK_H

#include <string>
#include <unistd.h>

using namespace std::chrono_literals;
SIMPLE_TASK_W_RESULT(RunCommandTask, {
  const char* cmd;
  bool disown = true;
}, {
  std::string std_out;
}, {
  log.min_level        = logging::INFO;
  std::string tmp_path = "/tmp/";
  tmp_path.append("cyd_ui_task_");
  tmp_path.append(std::to_string(get_id()));
  tmp_path.append(".out");
  
  // Run command
  std::string c;
  c.append("");
  c.append(this->args.cmd);
  c.append(" >");
  c.append(tmp_path);
  c.append(" 2>&1");
  if (args.disown) c.append(" & disown");
  log.debug("running $ > %s", c.c_str());
  std::system(c.c_str());
  
  // Retrieve STD OUTPUT
  std::ifstream out(tmp_path);
  if (out.is_open()) {
    result.std_out.clear();
    std::string token;
    while (out >> token) {
      result.std_out.append(token);
      result.std_out.append(" ");
    }
    out.close();
    std::remove(tmp_path.c_str());
    //log.info("[STDOUT] %s", result.std_out.c_str());
  }
})

#endif //CYD_UI_RUN_COMMAND_TASK_H
