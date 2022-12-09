//
// Created by castle on 8/21/22.
//

#include "../../include/window.hpp"
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
  int h
) {
  events::start();
  
  auto win = new CWindow();
  windows.push_back(win);
  listen(ResizeEvent, {
    graphics::resize(win->win_ref, it.data->w, it.data->h);
  })
  win->layout = layout;
  
  auto* win_ref = graphics::create_window(title, wclass, x, y, w, h);
  win->win_ref = win_ref;
  graphics::set_background(win_ref);
  
  layout->bind_window(win);
  return win;
}

