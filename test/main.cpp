//
// Created by castle on 8/21/22.
//

#include "../include/cydui.hpp"
#include "wifi/wifi.hpp"

#include <mcheck.h>
#include <thread>

int main() {
  mtrace();
  // Create layout tree
  
  // Instantiate window with layout
  auto* state = new TestState();
  auto* tc    = new Test(state, { }, in(Test, t, { }));
  
  auto                     * layout = new cydui::layout::Layout(tc);
  cydui::window::CWindow   * win    = cydui::window::create(
    layout,
    "test_window", "scratch",
    1, 29,
    //1, 13,
    1920, 30
  );
  cydui::graphics::window_t* w      = win->win_ref;
  
  // Interact with window through window pointer
  
  using namespace std::chrono_literals;
  while (1) {
    std::this_thread::sleep_for(10000s);
  }
  return 0;
}
