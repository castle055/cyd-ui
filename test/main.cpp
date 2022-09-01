//
// Created by castle on 8/21/22.
//

#include "../include/cydui.h"
#include "wifi/wifi.h"

#include <mcheck.h>
#include <thread>

int main() {
  mtrace();
  // Create layout tree
  
  // Instantiate window with layout
  auto* test_state = new TestState();
  auto layout = new cydui::layout::Layout(new TestComponent(test_state));
  cydui::window::CWindow   * win = cydui::window::create(
      layout,
      "test_window",
      "scratch",
      -1,
      -1,
      //1,
      //13,
      500,
      250
  );
  cydui::graphics::window_t* w   = win->win_ref;
  
  // Interact with window through window pointer
  
  using namespace std::chrono_literals;
  while (1) {
    std::this_thread::sleep_for(100s);
  }
  return 0;
}
