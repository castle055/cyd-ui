//
// Created by castle on 8/21/22.
//

#include "../../include/window.hpp"
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
  win->listener = new CWindowListener(win);
  events::subscribe(win->listener);
  win->layout = layout;
  
  auto* win_ref = graphics::create_window(title, wclass, x, y, w, h);
  win->win_ref = win_ref;
  graphics::set_background(win_ref);
  
  layout->bind_window(win);
  return win;
  test();
}

CWindowListener::CWindowListener(CWindow* win) {
  this->win = win;
}

void CWindowListener::on_event(cydui::events::CEvent* ev) {
  if (ev->win && this->win->win_ref->xwin != ev->win) return;
  switch (ev->type) {
    case events::EVENT_GRAPHICS:
      graphics::on_event(
        win->win_ref, (events::graphics::CGraphicsEvent*)(ev->data));
      break;
    case events::EVENT_LAYOUT:win->layout->on_event((events::layout::CLayoutEvent*)(ev->data));
      break;
    default: break;
  }
}
