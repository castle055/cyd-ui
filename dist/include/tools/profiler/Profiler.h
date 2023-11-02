//
// Created by castle on 10/19/23.
//

#ifndef CYD_UI_PROFILER_H
#define CYD_UI_PROFILER_H

#include "../../cydui.hpp"
#include "../../containers.hpp"
#include "ThreadTimeline.h"

STATE(Profiler) {
  prof::time_point now = prof::PROFILING_CLOCK::now();
  double time_scale = 1.0 / 10000.0;
  
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
    if (prof::PROFILING_CLOCK::now() > state->now) {
      state->now = prof::PROFILING_CLOCK::now() + (25 * 16666us);
      state->time_scale = dim->cw.val() / (3 * 25.0 * 16666.0);
    }
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
          FOR_EACH(ThreadTimeline)(props.ctx.threads, [this](auto &thd_data) {
            return cydui::components::c_init_t<ThreadTimeline> {
              .props = {
                .ctx = props.ctx,
                .thd_id = thd_data.first,
                .now = state->now,
                .time_scale = state->time_scale,
              },
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
