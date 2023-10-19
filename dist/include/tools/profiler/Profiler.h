//
// Created by castle on 10/19/23.
//

#ifndef CYD_UI_PROFILER_H
#define CYD_UI_PROFILER_H

#include "../../cydui.hpp"
#include "../../containers.hpp"
#include "ThreadTimeline.h"

STATE(Profiler) {
  double time_scale = 1.0 / 1000.0;
  
  INIT_STATE(Profiler) {
  
  }
};

COMPONENT(Profiler) {
  PROPS({
    prof::context_t &ctx;
  })
  INIT(Profiler) {
  
  }
  
  REDRAW {
    auto now = prof::PROFILING_CLOCK::now();
    add({
      COMP(VBox)({
        .props = {
          .spacing = 5,
        },
        .x = 0,
        .y = 0,
        .w = dim->w,
        .h = dim->h,
        .inner = {
          FOR_EACH(ThreadTimeline)(props.ctx.threads, [this, now](auto &thd_data) {
            return cydui::components::c_init_t<ThreadTimeline> {
              .props = {
                .ctx = props.ctx,
                .thd_id = thd_data.first,
                .now = now,
                .time_scale = state->time_scale,
              },
              .x = 0,
              .y = 0,
              .w = dim->w,
              .h = 50,
            };
          }),
        }
      }),
    });
  };
};

#endif //CYD_UI_PROFILER_H
