// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

#include "cyd_ui/window.h"
#include "cyd_ui/layout.h"
#include <vector>

using namespace cyd::ui::window;

void cyd::ui::window::CWindow::terminate() {
  delete layout;
  graphics::terminate(win_ref);
  win_ref = nullptr;
}

std::pair<int, int> cyd::ui::window::CWindow::get_position() const {
  return graphics::get_position(win_ref);
}
std::pair<int, int> cyd::ui::window::CWindow::get_size() const {
  return graphics::get_size(win_ref);
}

CWindow* cyd::ui::window::create(
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
  auto* win_ref = graphics::create_window(win, &win->profiling_ctx, title, wclass, x, y, w, h, override_redirect);
  win->layout = layout;
  win->win_ref = win_ref;
  
  layout->bind_window(win);
  
  // Once all threads have started and everything is set up for this window
  // force a complete redraw
  //events::emit<RedrawEvent>({.win = (unsigned int) win_ref->xwin});
  //events::emit<RedrawEvent>({ });
  //events::emit<RedrawEvent>({.win = get_id(win_ref)});
  
  return win;
}

