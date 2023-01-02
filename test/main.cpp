//
// Created by castle on 8/21/22.
//

#include "../include/cydui.hpp"
#include "wifi/wifi.hpp"
#include "status_bar/workspaces.hpp"
#include "status_bar/window_title.hpp"

#include <mcheck.h>
#include <thread>

using namespace std::chrono_literals;

int main() {
  //mtrace();
  // Create layout tree
  
  // Instantiate window with layout
  auto* state = new WorkspacesState();
  state->selected_workspaces.on_change([state]() {
    state->selected_workspaces.set(state->selected_workspaces.val() ^ 1);
  });
  //  auto* state1 = new WinTitleState();
  //  auto* state2 = new WinTitleState();
  auto* tc = new Workspaces(state, { }, in(Workspaces, t, { }));
  //  auto* tc1    = new WinTitle(state1, { }, in(Button, t1, { }));
  //  auto* tc2    = new WinTitle(state2, { }, in(Button, t2, { }));
  
  auto* layout = new cydui::layout::Layout(tc);
  //  auto* layout1 = new cydui::layout::Layout(tc1);
  //  auto* layout2 = new cydui::layout::Layout(tc2);
  
  cydui::window::CWindow   * win = cydui::window::create(
    layout,
    "workspaces", "scratch",
    0, 50,//100, 260,
    //1, 13,
    280, 25,
    true
  );
  //  cydui::window::CWindow* win1 = cydui::window::create(
  //    layout1,
  //    "window_title", "scratch",
  //    600, 26,
  //    //1, 13,
  //    250, 25
  //  );
  //  cydui::window::CWindow* win2 = cydui::window::create(
  //    layout2,
  //    "status_bar", "scratch",
  //    -1, 26,
  //    //1, 13,
  //    700, 25
  //  );
  //
  cydui::graphics::window_t* w   = win->win_ref;
  //  cydui::graphics::window_t* w1 = win1->win_ref;
  //  cydui::graphics::window_t* w2 = win2->win_ref;
  
  // Interact with window through window pointer
  
  while (1) {
    std::this_thread::sleep_for(10000s);
  }
  return 0;
}
