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

STATE(TestComponent) {
  int off = 0.0;
  INIT_STATE(TestComponent) {
  }
};


void asdfgg(std::string s) {

}

struct something {
  int num;
  
  std::string str() const {
    return std::to_string(num);
  }
};

COMPONENT(TestComponent) {
  PROPS({
  })
  INIT(TestComponent) {
    ENABLE_LOG
  }
  REDRAW {
    asdfgg("fdas");
    asdfgg(something{1234234}.str());
    asdfgg(something{34}.str());
    //auto f = {it -> return it.first };
    //auto d = [](auto it){ return it.first; };
    add({
      COMP(Canvas)({
        .props = {
          .draw = [this](vg::vg_fragment_t &gc) {
            for (int i = 0; i < dim->cw.val() / 8; ++i) {
              gc.append(
                vg::line {}
                  .stroke({.color = "#FCAE1E"_color})
                  .x1(int {i * 8})
                  .y1(0)
                  .x2(dim->cw.val() - i * 8)
                  .y2(dim->ch.val())
                  .stroke_dashoffset(state->off)
                  .stroke_dasharray({20, 10, 2})
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
        },
        .w = dim->cw,
        .h = dim->ch,
      }),
    });
  };
  
  ON_SCROLL(dx, dy) {
    state->off += dy / 64;
    cydui::events::emit<RedrawEvent>({
      .win = cydui::graphics::get_id(*state->win.unwrap()),
      .component = state,
    });
  }
};

inline consteval int counter() {
  return __LINE__;
}

int main() {
  int a = counter();
  int b = counter();
  int c = counter();
  
  
  cydui::window::CWindow* win = cydui::window::create(
    cydui::layout::create<TestComponent>({
    }),
    "test",
    "scratch",
    0, 25,//100, 260,
    800, 800,
    false
  );
  
  cydui::window::CWindow* pwin = cydui::window::create(
    cydui::layout::create<Profiler>({
      .props = {
        .ctx = win->profiling_ctx,
      }
    }),
    "Profiler",
    "scratch",
    0, 25,//100, 260,
    1280, 400,
    false
  );
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
    cydui::events::emit<RedrawEvent>({
      .win = cydui::graphics::get_id(pwin->win_ref)
    });
    std::this_thread::sleep_for(16666us);
  }
  return 0;
}
