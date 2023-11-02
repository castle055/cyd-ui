//
// Created by castle on 10/19/23.
//

#ifndef CYD_UI_THREADTIMELINE_H
#define CYD_UI_THREADTIMELINE_H

#include <ranges>

#include "../../cydui.hpp"

STATE(ThreadTimeline) {
  INIT_STATE(ThreadTimeline) {
  
  }
};

COMPONENT(ThreadTimeline) {
  PROPS({
    prof::context_t &ctx;
    std::thread::id thd_id;
    prof::time_point now;
    double time_scale = 1.0;
    int point_event_radius = 6;
  })
  INIT(ThreadTimeline) {
  
  }
  
  REDRAW {
    
    add({
      COMP(Canvas)({
        .props = {
          .draw = [this](vg::vg_fragment_t &frag) {
            auto &thd_ctx = props.ctx.threads[props.thd_id];
            
            // Paint timeline
            frag.append(
              vg::line {}
                .stroke({.color = "#FCAE1E"_color})
                .x1(0)
                .y1(dim->ch.val() / 2)
                .x2(dim->cw.val())
                .y2(dim->ch.val() / 2)
            );
            
            // Paint events
            int x = 0;
            int w = 0;
            for (const auto &ev: std::ranges::reverse_view(thd_ctx.timeline)) {
              if (nullptr == ev.description || '\0' == *ev.description) continue;
              x = dim->cw.val() - (int) ((double) prof::duration_cast<prof::microseconds>(
                props.now - ev.t0).count() * props.time_scale);
              if (ev.t1.has_value()) {
                w = (int) (((double) prof::duration_cast<prof::microseconds>(
                  ev.t1.value() - ev.t0).count()) * props.time_scale);
                if (x + w > 0) {
                  frag.append(
                    vg::rect {}
                      .x(x)
                      .y(0)
                      .width(int {std::max(w, 1)})
                      .height(dim->ch.val())
                      .fill({.color = "#FCAE1E"_color})
                  );
                }
              } else {
                if (x + w > 0) {
                  frag.append(
                    vg::circle {}
                      .cx((x < 0 ? 0 : x) - props.point_event_radius)
                      .cy((dim->ch.val() / 2) - props.point_event_radius)
                      .r(int {props.point_event_radius})
                      .fill({.color = "#00CFCC"_color})
                  );
                }
              }
            }
          }
        },
        .w = dim->cw,
        .h = dim->ch,
      })
    });
  };
};

#endif //CYD_UI_THREADTIMELINE_H
