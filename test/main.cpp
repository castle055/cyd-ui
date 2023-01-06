//
// Created by castle on 8/21/22.
//

#include "../include/cydui.hpp"
#include "wifi/wifi.hpp"
#include "startmenu/startmenu.hpp"
#include "status_bar/workspaces.hpp"
#include "status_bar/window_title.hpp"

#include <mcheck.h>
#include <thread>

using namespace std::chrono_literals;

int main() {
  //mtrace();
  // Create layout tree
  
  // Instantiate window with layout
  auto* state = new StartMenuState();
  //auto* state1 = new WorkspacesState();
  //state->selected_workspaces.on_change([state]() {
  //  state->selected_workspaces.set(state->selected_workspaces.val() ^ 1);
  //});
  auto* tc    = new StartMenu(state, { }, ___inner(StartMenu, t, { }));
  //auto* tc1    = new Workspaces(state1, { }, ___inner(Workspaces, t, { }));
  
  auto* layout = new cydui::layout::Layout(tc);
  //auto* layout1 = new cydui::layout::Layout(tc1);
  
  cydui::window::CWindow* win = cydui::window::create(
    layout,
    "startmenu", "scratch",
    1920, 25,//100, 260,
    //1, 13,
    700, 800,
    true
  );
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
