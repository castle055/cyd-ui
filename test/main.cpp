//
// Created by castle on 8/21/22.
//

#include "../include/cydui.hpp"
#include <mcheck.h>
#include <thread>
#include "cydstd/nullable.h"
#include <cydstd/loops.h>
#include "tools/profiler/Profiler.h"

using namespace std::chrono_literals;

STATE(Canvas) {

};
COMPONENT(Canvas, {
  std::function<void(vg::vg_fragment_t &fragment)> draw_fn;
}) {
  FRAGMENT {
    props->draw_fn(fragment);
  };
};

STATE(TestComponent) {
  int off = 0.0;
};
COMPONENT(TestComponent, { }) {
  ON_REDRAW {
    return {
      Canvas {{
        .draw_fn = [this](vg::vg_fragment_t &gc) {
          //! Note that here dimensional variable (ie: cw) refer to those
          //! from TestComponent and not Canvas because the lambda captures 'this'
          for (int i = 0; i <= $cw().val() / 8; ++i) {
            gc.append(
              vg::line {}
                .stroke({.color = "#FCAE1E"_color})
                .x1(int {i * 8})
                .y1(0)
                .x2(($cw().val() / 8 - i) * 8)
                .y2($ch().val())
              //.stroke_dashoffset(state->off)
              //.stroke_dasharray({20, 10, 2})
            );
          }
          
          gc.append(
            vg::polyline {}
              .stroke_width(3)
              .stroke({.color = "#FCAE1E"_color})
              .fill({.color = "#AC7E0E"_color})
              .points({
                {0, 300},
                {30, 300},
                {30, 350},
                {80, 400},
                {30, 450},
                {30, 500},
                {0, 500},
              })
          );
        }
      }}.w($cw())
        .h($ch()),
    };
  };
  
  ON_SCROLL {
    state->off += dy / 64;
    this->
    //cydui::events::emit<RedrawEvent>({
    //  .win = cydui::graphics::get_id(*state->win.unwrap()),
    //  .component = state,
    //});
  }
};


int main() {
  
  cydui::window::CWindow* win = cydui::window::create(
    cydui::layout::create<TestComponent>({
    }),
    "test",
    "scratch",
    0, 25,//100, 260,
    800, 800,
    false
  );
  
  //cydui::window::CWindow* pwin = cydui::window::create(
  //  cydui::layout::create<Profiler>({
  //    .props = {
  //      .ctx = win->profiling_ctx,
  //    }
  //  }),
  //  "Profiler",
  //  "scratch",
  //  0, 25,//100, 260,
  //  1280, 400,
  //  false
  //);
  //
  //cydui::events::on_event<RedrawEvent>(cydui::events::Consumer<RedrawEvent>(
  //  [=](const cydui::events::ParsedEvent<RedrawEvent> &it) {
  //    if (it.data->win == cydui::graphics::get_id(win->win_ref)) {
  //      cydui::events::emit<RedrawEvent>({
  //        .win = cydui::graphics::get_id(pwin->win_ref)
  //      });
  //    }
  //  }));
  
  for (ever) {
    //cydui::events::emit<RedrawEvent>({
    //  .win = cydui::graphics::get_id(pwin->win_ref)
    //});
    std::this_thread::sleep_for(16666us);
  }
  return 0;
}
