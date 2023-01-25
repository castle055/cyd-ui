//
// Created by castle on 8/21/22.
//

#include "../include/cydui.hpp"

#include <mcheck.h>
#include <thread>

using namespace std::chrono_literals;

STATE(Test) {
  cydui::layout::color::Color* c = new cydui::layout::color::Color("#FCAE1E");
  cydui::layout::fonts::Font font {
    .name = "Fira Code Retina",
    .size = 10
  };
  
};
COMPONENT(Test) {
  NO_PROPS INIT(Test) {
  }
  REDRAW {
    HBox* hbox_ref;
    add({
      COMP(HBox)({
        .ref = &hbox_ref,
        .props = {
          .spacing = 10,
        },
        .x = 10,
        .y = 10,
        .inner = {
          COMP(VBox)({
            .props = {
              .spacing = 10,
            },
            .inner = {
              COMP(Rectangle)({
                .props = {
                  .color = state->c,
                  .filled = true,
                },
                .w = dim->cw / 2 - 15,
                .h = dim->ch / 4,
              }),
              COMP(Rectangle)({
                .props = {
                  .color = state->c,
                  .filled = true,
                },
                .w = dim->cw / 2 - 15,
                .h = 40,
              }),
              COMP(Text)({
                .props = {
                  .color = state->c,
                  .font = &state->font,
                  .text = "Test TEXT",
                },
              }),
              COMP(Rectangle)({
                .props = {
                  .color = state->c,
                  .filled = true,
                },
                .w = dim->cw / 2 - 15,
                .h = dim->ch / 4,
              }),
            }
          }),
          COMP(VBox)({
            .props = {
              .spacing = 10,
            },
            .inner = {
              COMP(Rectangle)({
                .props = {
                  .color = state->c,
                  .filled = true,
                },
                .w = dim->cw / 2 - 15,
                .h = dim->ch / 4,
              }),
              COMP(Rectangle)({
                .props = {
                  .color = state->c,
                  .filled = true,
                },
                .w = dim->cw / 2 - 15,
                .h = 50,
              }),
            }
          })
        },
      }),
    });
  }
};

int main() {
  //mtrace();
  // Create layout tree
  
  // Instantiate window with layout
  auto* state = new TestState();
  //auto* state1 = new WorkspacesState();
  //state->selected_workspaces.on_change([state]() {
  //  state->selected_workspaces.set(state->selected_workspaces.val() ^ 1);
  //});
  auto* tc = new Test(state, {}, ___inner(Test, t, {
  }));
  //auto* tc1    = new Workspaces(state1, { }, ___inner(Workspaces, t, { }));
  
  auto* layout = new cydui::layout::Layout(tc);
  //auto* layout1 = new cydui::layout::Layout(tc1);
  
  cydui::window::CWindow* win = cydui::window::create(layout,
    "startmenu",
    "scratch",
    0, 25,//100, 260,
    //1, 13,
    300, 200,
    false);
  //cydui::window::CWindow   * win1 = cydui::window::create(
  //  layout1,
  //  "workspaces", "scratch",
  //  0, 90,//100, 260,
  //  //1, 13,
  //  280, 25,
  //  true
  //);
  
  cydui::graphics::window_t* w = win->win_ref;
  
  // Interact with window through window pointer
  
  while (1) {
    std::this_thread::sleep_for(10000s);
  }
  return 0;
}
