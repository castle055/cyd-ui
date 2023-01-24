//
// Created by castle on 1/14/23.
//

#ifndef CYD_UI_SYSMON_TASKS_HPP
#define CYD_UI_SYSMON_TASKS_HPP

#include "../../../../../include/cydui.hpp"
#include "../../../../tasks/run_command_task.h"

#include <utility>
#include <sys/sysinfo.h>
#include <sys/statfs.h>
#include <fstream>

using namespace std::chrono_literals;

SIMPLE_TASK_W_RESULT(SysMonTask, {
}, {
  int cpu = 0;
  int mem = 0;
}, {
  // MEM
  struct sysinfo info { };
  std::ifstream  meminfo("/proc/meminfo");
  long           total = 0;
  long           avail = 0;
  std::string    token;
  while (meminfo >> token) {
    if (token == "MemTotal:") {
      meminfo >> total;
    } else if (token == "MemAvailable:") {
      meminfo >> avail;
    }
    if (total > 0 && avail > 0)
      break; // both have been found, stop parsing
  }
  result.mem = (100 * (total - avail)) / total;
  
  // CPU
  if (sysinfo(&info) == 0) {
    result.cpu = (info.loads[0] / get_nprocs()) / 1000;
  }
})

SIMPLE_TASK_W_RESULT(SysMonDisksTask, {
}, {
  int storage = 0;
  int disk0   = 0;
  int disk1   = 0;
  int disk2   = 0;
}, {
  // DISKS
  struct statfs64 stat;
  statfs64("/", &stat);
  result.disk0 = 100 - (100 * stat.f_bavail) / stat.f_blocks;
  statfs64("/home", &stat);
  result.disk1 = 100 - (100 * stat.f_bavail) / stat.f_blocks;
  statfs64("/data", &stat);
  result.disk2 = 100 - (100 * stat.f_bavail) / stat.f_blocks;
  //log.debug("data AYE %d", result.disk2);
  
  result.storage = result.disk0;
})

SIMPLE_TASK_W_RESULT(SysMonTempsTask, {
}, {
  int cpu_tmp  = 0;
  int disk_tmp = 0;
}, {
  RunCommandTask run_command_task;
  // TODO - This way it is synchronized, damn... sounds like a promise
  run_command_task.set_args({
    "sensors",
    false
  });
  run_command_task.exec();
  std::string token;
  auto* r = run_command_task.res();
  int t_i = r->std_out.find("Tdie:");
  int ct  = std::stoi(r->std_out.substr(t_i + 7, 2));
  result.cpu_tmp = ct;
  int t1_i = r->std_out.find("Composite:");
  int dt   = std::stoi(r->std_out.substr(t1_i + 12, 2));
  result.disk_tmp = dt;
})

#endif //CYD_UI_SYSMON_TASKS_HPP
