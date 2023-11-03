//
// Created by castle on 8/21/22.
//

#include "core/window.h"
#include "core/layout.h"
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
  auto* win_ref = graphics::create_window(&win->profiling_ctx, title, wclass, x, y, w, h, override_redirect);
  win->layout = layout;
  
  win->win_ref = win_ref;
  //graphics::set_background(win_ref);
  
  layout->bind_window(win);
  
  windows.push_back(win);
  
  events::start();
  
  // Once all threads have started and everything is set up for this window
  // force a complete redraw
  //events::emit<RedrawEvent>({.win = (unsigned int) win_ref->xwin});
  //events::emit<RedrawEvent>({ });
  //events::emit<RedrawEvent>({.win = get_id(win_ref)});
  
  return win;
}

