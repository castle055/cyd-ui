//
// Created by castle on 8/21/22.
//

#include "window.hpp"
#include "../graphics/events.hpp"
#include <vector>

using namespace cydui::window;

static std::vector<CWindow*> windows;

CWindow* cydui::window::create(
  layout::Layout* layout,
  const char* title,
  const char* wclass,
  int x,
  int y,
  int w,
  int h,
  bool override_redirect
) {
  auto win = new CWindow();
  auto* win_ref = graphics::create_window(title, wclass, x, y, w, h, override_redirect);
  win->layout = layout;
  
  win->win_ref = win_ref;
  graphics::set_background(win_ref);
  
  layout->bind_window(win);
  
  windows.push_back(win);
  cydui::events::on_event<ResizeEvent>(
    cydui::events::Consumer<ResizeEvent>([=](const cydui::events::ParsedEvent<ResizeEvent> &it) {
      graphics::resize(win_ref, it.data->w, it.data->h);
    })
  );
  
  events::start();
  
  // Once all threads have started and everything is set up for this window
  // force a complete redraw
  events::emit<RedrawEvent>({ });
  
  return win;
}

