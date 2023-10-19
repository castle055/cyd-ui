//
// Created by castle on 10/5/23.
//

#ifndef CYD_UI_SPLINETEST_H
#define CYD_UI_SPLINETEST_H

#include "../include/cydui.hpp"

#include "../include/splines.h"

STATE(SplineTest) {
  double x[4] = {0, 1, 3, 4};
  double y[4] = {0, 2, 2, 0};
  
  double t = 0.0;
  
  INIT_STATE(SplineTest) {
  }
};

COMPONENT(SplineTest) {
  PROPS({
    
  })
  
  INIT(SplineTest) {
    //ENABLE_LOG
  }
  
  REDRAW {
    state->t += 0.01;
    if (state->t >= 1) {
      state->t = 0;
    }
    log.debug("t = %.2f", state->t);
  }
  
  RENDER {
    //using namespace std::chrono_literals;
    //std::this_thread::sleep_for(1s);
    static int scale = 50;
    
    double x, y;
    for (std::size_t i = 0; i < 4; ++i) {
      x = state->x[i];
      y = state->y[i];
      graphics::drw_arc(win,
        "#FCAE1E"_color,
        dim->cx.val() + x * scale - 5,
        dim->cy.val() + y * scale - 5,
        10, 10,
        0, 360,
        true
      );
    }
    
    for (double t = 0; t <= 1; t += 0.001) {
      x = cubic_bezier(
        state->x[0],
        state->x[1],
        state->x[2],
        state->x[3],
        t
      );
      y = cubic_bezier(
        state->y[0],
        state->y[1],
        state->y[2],
        state->y[3],
        t
      );
      
      graphics::drw_arc(win,
        "#ACCE8E"_color,
        dim->cx.val() + x * scale - 1,
        dim->cy.val() + y * scale - 1,
        2, 2,
        0, 360,
        true
      );
    }
  }
};


#endif //CYD_UI_SPLINETEST_H
