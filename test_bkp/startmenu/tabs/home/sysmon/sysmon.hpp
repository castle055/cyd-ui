//
// Created by castle on 1/10/23.
//

#ifndef CYD_UI_SYSMON_HPP
#define CYD_UI_SYSMON_HPP

#include "../../../../../include/cydui.hpp"
#include "../../../../components/button.hpp"
#include "../../../../components/flexbox.hpp"
#include "../../../../components/clock_module.hpp"
#include "../../listview.hpp"
#include "../../appbutton.hpp"
#include "../../../config/startappscm.hpp"
#include "../radial_indicator.hpp"
#include "../bar_indicator.hpp"
#include "./sysmon_tasks.hpp"
#include <map>

using namespace std::chrono_literals;

STATE(SysMon)
  cydui::layout::color::Color* c_fg = new cydui::layout::color::Color("#FCAE1E");
  int                             value = 100;
  tasks::timer_t<SysMonTask>      sysmon_task;
  tasks::timer_t<SysMonDisksTask> sysmon_disks_task;
  tasks::timer_t<SysMonTempsTask> sysmon_temps_task;
  
  INIT_STATE(SysMon) {
    sysmon_task.start({
    }, 1s, -1, true);
    sysmon_disks_task.start({
    }, 10s, -1, true);
    sysmon_temps_task.start({
    }, 3s, -1, true);
    //run_command_task.run({"echo hello world"});
  }
};

COMPONENT(SysMon)
  NO_PROPS INIT(SysMon) }
  
  REDRAW {
    auto* sys_info  = state->sysmon_task.res();
    auto* disk_info = state->sysmon_disks_task.res();
    auto* temp_info = state->sysmon_temps_task.res();
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
      COMP(FlexBox)({
        .inner = {
          COMP(RadialIndicator)({
            .props = {
              .text = "CPU",
              .value = temp_info->cpu_tmp,
              .show_value = true,
            },
          }),
          COMP(RadialIndicator)({
            .props = {
              .text = "DSK",
              .value = temp_info->disk_tmp,// - 23,
              .show_value = true,
            },
          }),
        },
        .init = [this](FlexBox* f) {
          f->set_pos(this, 15, 150);
          f->set_width(this->state->geom.content_w().val() - 30);
        },
        // TODO - FIXME - THIS DOES NOT WORK BECAUSE IT NEEDS THE ADD CALL TO HAVE BEEN EXECUTED FIRST
        //.w = this->state->geom.content_w().val(),
      }),
    });
    set_padding(10, 10, 14, 10);
    set_border_enable(true);
  }
  
};

#endif //CYD_UI_SYSMON_HPP
