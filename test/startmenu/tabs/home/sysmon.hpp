//
// Created by castle on 1/10/23.
//

#ifndef CYD_UI_SYSMON_HPP
#define CYD_UI_SYSMON_HPP

#include "../../../../include/cydui.hpp"
#include "../../../components/button.hpp"
#include "../../../components/flexbox.hpp"
#include "../../../components/clock_module.hpp"
#include "../listview.hpp"
#include "../appbutton.hpp"
#include "../../config/startappscm.hpp"
#include "./radial_indicator.hpp"
#include "./bar_indicator.hpp"
#include <map>
#include <utility>
#include <sys/sysinfo.h>
#include <sys/statfs.h>

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

STATE(SysMon)
  cydui::layout::color::Color* c_fg = new cydui::layout::color::Color("#FCAE1E");
  int                             value = 100;
  tasks::timer_t<SysMonTask>      sysmon_task;
  tasks::timer_t<SysMonDisksTask> sysmon_disks_task;
  
  INIT_STATE(SysMon) {
    sysmon_task.start({
    }, 1s, -1, true);
    sysmon_disks_task.start({
    }, 10s, -1, true);
  }
};

COMPONENT(SysMon)
  NO_PROPS INIT(SysMon) }
  
  REDRAW {
    auto* sys_info  = state->sysmon_task.res();
    auto* disk_info = state->sysmon_disks_task.res();
    state->value--;
    if (state->value == 0)
      state->value += 100;
    
    using namespace primitives;
    add({
      COMP(FlexBox)({
        .inner = {
          COMP(RadialIndicator)({
            .props = {
              .text = "CPU",
              .value = sys_info->cpu,
              .show_value = true,
            },
          }),
          COMP(RadialIndicator)({
            .props = {
              .text = "MEM",
              .value = sys_info->mem,// - 23,
              .show_value = true,
            },
          }),
          COMP(RadialIndicator)({
            .props = {
              .text = "STG",
              .value = disk_info->disk0,// - 95,
              .show_value = true,
            },
          }),
        },
        .init = [this](FlexBox* f) {
          f->set_width(this->state->geom.content_w().val());
        },
        // TODO - FIXME - THIS DOES NOT WORK BECAUSE IT NEEDS THE ADD CALL TO HAVE BEEN EXECUTED FIRST
        //.w = this->state->geom.content_w().val(),
      }),
      COMP(FlexBox)({
        .props = {
          .vertical = true,
        },
        .inner = {
          COMP(BarIndicator)({
            .props = {
              .text = "/",
              .value = disk_info->storage,
              .show_value = true,
            },
            .init = [](BarIndicator* bi) {
              bi->set_size(125, 8);
            },
          }),
          COMP(BarIndicator)({
            .props = {
              .text = "/HOME",
              .value = disk_info->disk1,
              .show_value = true,
            },
            .init = [](BarIndicator* bi) {
              bi->set_size(125, 8);
            },
          }),
          COMP(BarIndicator)({
            .props = {
              .text = "/DATA",
              .value = disk_info->disk2,
              .show_value = true,
            },
            .init = [](BarIndicator* bi) {
              bi->set_size(125, 8);
            },
          }),
          // TODO - write TimelineComponents and put one here
          COMP(BarIndicator)({
            .props = {
              .value = state->value,
            },
            .init = [](BarIndicator* bi) {
              bi->set_size(125, 12);
            },
          }),
        },
        .init = [this](FlexBox* f) {
          f->set_pos(this, 0, 60);
          f->set_height(85);
        },
      }),
    });
    set_padding(10, 10, 14, 10);
    set_border_enable(true);
  }
  
};

#endif //CYD_UI_SYSMON_HPP
