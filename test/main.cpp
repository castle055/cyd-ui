//
// Created by castle on 8/21/22.
//

#include "../include/cydui.hpp"
#include "../include/containers.hpp"

#include <mcheck.h>
#include <thread>

#include "cydstd/nullable.h"
#include "primitives.hpp"

#include "terminal_button.hpp"

using namespace std::chrono_literals;

namespace test {
    STATE(Test) {
      color::Color c = "#FCAE1E"_color;
      //color::Color c = color::Red;
      color::Color c1 = "#111A1E"_color;
      font::Font font {.name = "Fira Code Retina", .size = 14};
      
      int scroll = 0;
    };
    COMPONENT(Test) {
      PROPS({
        int a = 4;
      })
      int b = 4;
      INIT(Test) {
        ENABLE_LOG
      }
      REDRAW {
        HBoxState* hbox_ref;
        bool teast = 10;
        //this->parent = this;
        //this->parent->dim->given_h = true;
        //this->parent->dim = nullptr;
        add({
          COMP(TerminalButton)({
            .x = 20,
            .y = 450,
          }),
          //[this]() {
          //  return COMP(Rectangle)({
          //
          //  });
          //}(),
          //IF((1 & 2) > 0, Rectangle)({
          //
          //}),
          //IF((1 & 3) > 0) COMP(Rectangle)({});
          //ELSE_IF(-1 < 0) COMP(Rectangle)({});
          //ELSE_IF(1 < 0) COMP(Rectangle)({});
          //  ELSE COMP(Circle)({});
          //  END,
          //WHEN(teast)
          //  CASE(false) COMP(Rectangle)({});
          //  CASE(true) COMP(Circle)({});
          //  END,
          COMP(Rectangle)({
            .props = {
              .color  = state->c,
              .filled = true,
            },
            .w = 250,
            .h = 50,
          }),
          COMP(VBox)({
            .props = {
              .spacing = 10,
            },
            .y = 60,
            .inner = {
              COMP(ViewPort)({
                .props = {
                  .x = 0,
                  .y = state->scroll,
                },
                .x = 50,
                .w = 100,
                .h = 100,
                .inner = {
                  COMP(Text)({
                    .props = {
                      .color = state->c,
                      .font  = &state->font,
                      .text  = "Test TEXT",
                    },
                  }),
                  COMP(VBox)({
                    .props = {
                      .spacing = 10,
                    },
                    .inner = {
                      COMP(Text)({
                        .props = {
                          .color = state->c,
                          .font  = &state->font,
                          .text  = "Test TEXT",
                        },
                      }),
                      COMP(Text)({
                        .props = {
                          .color = state->c,
                          .font  = &state->font,
                          .text  = "Test TEXT",
                        },
                      }),
                      COMP(Text)({
                        .props = {
                          .color = state->c,
                          .font  = &state->font,
                          .text  = "Test TEXT",
                        },
                      }),
                      COMP(Text)({
                        .props = {
                          .color = state->c,
                          .font  = &state->font,
                          .text  = "Test TEXT",
                        },
                      }),
                      COMP(Text)({
                        .props = {
                          .color = state->c,
                          .font  = &state->font,
                          .text  = "Test TEXT",
                        },
                      }),
                      COMP(Text)({
                        .props = {
                          .color = state->c,
                          .font  = &state->font,
                          .text  = "Test TEXT",
                        },
                      }),
                      COMP(Text)({
                        .props = {
                          .color = state->c,
                          .font  = &state->font,
                          .text  = "Test TEXT",
                        },
                      }),
                      COMP(Text)({
                        .props = {
                          .color = state->c,
                          .font  = &state->font,
                          .text  = "Test TEXT",
                        },
                      }),
                      COMP(Text)({
                        .props = {
                          .color = state->c,
                          .font  = &state->font,
                          .text  = "Test TEXT",
                        },
                      }),
                      COMP(Text)({
                        .props = {
                          .color = state->c,
                          .font  = &state->font,
                          .text  = "Test TEXT",
                        },
                      }),
                      COMP(Text)({
                        .props = {
                          .color = state->c,
                          .font  = &state->font,
                          .text  = "Test TEXT",
                        },
                      }),
                      COMP(Text)({
                        .props = {
                          .color = state->c,
                          .font  = &state->font,
                          .text  = "Test TEXT",
                        },
                      }),
                    },
                  }),
                }
              }),
            },
          }),
          //COMP(Image)({
          //  .props = {
          //    .img = "/home/castle/pics/prof.jpg",
          //  },
          //  .w = dim->cw,
          //}),
          //COMP(HBox)({
          //  .ref = &hbox_ref,
          //  .props = {
          //    .spacing = 10,
          //  },
          //  .x     = 10,
          //  .y     = 10,
          //  .inner = {
          //    COMP(VBox)({
          //      .props = {
          //        .spacing = 10,
          //      },
          //      .inner = {
          //COMP(Rectangle)({
          //  .props = {
          //    .color  = state->c1,
          //    .filled = true,
          //  },
          //  .w = dim->cw / 2 - 15,
          //  .h = dim->ch / 4,
          //}),
          //COMP(Rectangle)({
          //  .props = {
          //    .color  = state->c,
          //    .filled = true,
          //  },
          //  .w = dim->cw / 2 - 15,
          //  .h = 40,
          //}),
          //COMP(VBox)({
          //  .props = {
          //    .spacing = 10,
          //  },
          //  .inner = {
          //    COMP(Rectangle)({
          //      .props = {
          //        .color  = state->c,
          //        .filled = true,
          //      },
          //      .w = dim->cw / 2 - 15,
          //      .h = dim->ch / 4,
          //    }),
          //    COMP(Rectangle)({
          //      .props = {
          //        .color  = state->c,
          //        .filled = true,
          //      },
          //      .w = dim->cw / 2 - 15,
          //      .h = 50,
          //    }),
          //  }
          //}),
          //      },
          //    }),
          //  },
          //})
        });
      };
      ON_SCROLL(dx, d) {
        //log.info("SCROLL: %dy", dy);
        state->scroll += d > 0 ? 10 : (d < 0 ? -10 : 0);
        state->dirty();
      }
    };
}

int main() {
  //mtrace();
  
  cydui::layout::Layout* layout = cydui::layout::create < test::Test > ({
  });
  cydui::window::CWindow* win = cydui::window::create(layout,
    "startmenu",
    "scratch",
    0,
    25,//100, 260,
    //1, 13,
    800,
    800,
    false);
  
  while (1) {
    std::this_thread::sleep_for(10000s);
  }
  return 0;
}
