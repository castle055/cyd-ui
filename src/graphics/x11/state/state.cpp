//
// Created by castle on 8/21/22.
//

#include "state.hpp"
#include "cydstd/logging.hpp"

const logging::logger log_task = {.name = "X11_IMPL::state"};

static Display* dpy = nullptr;
static int screen = -1;

using namespace x11;

Display* state::get_dpy() {
  if (dpy)
    return dpy;
  if ((dpy = XOpenDisplay(nullptr))) {
    log_task.info("dpy = %lX", (unsigned long) dpy);
    return dpy;
  } else {
    log_task.error("Could not open display");
    return nullptr;
  }
}

int state::get_screen() {
  if (screen != -1)
    return screen;
  screen = DefaultScreen(get_dpy());
  log_task.info("screen = %d", screen);
  return screen;
}

Window state::get_root() {
  Window root = XRootWindow(get_dpy(), get_screen());
  log_task.info("root = %lu", root);
  return root;
}
