/*! \file  state.cppm
 *! \brief 
 *!
 */

module;
#include <X11/Xlib.h>
#include <clocale>

export module cydui.graphics.native:state;

import fabric.logging;
export import fabric.async;
export import fabric.profiling;

export import cydui.graphics.types;
export import cydui.threading;

static Display* dpy = nullptr;
static int screen = -1;

export namespace x11::state {
    Display* get_dpy() {
      if (dpy)
        return dpy;
      if ((dpy = XOpenDisplay(nullptr))) {
        setlocale(LC_ALL, "");
        LOG::print{INFO}("dpy = {}", (unsigned long) dpy);
        return dpy;
      } else {
        LOG::print{ERROR}("Could not open display");
        return nullptr;
      }
    }

    int get_screen() {
      if (screen != -1)
        return screen;
      screen = DefaultScreen(get_dpy());
      LOG::print{INFO}("screen = {}", screen);
      return screen;
    }

    Window get_root() {
      Window root = XRootWindow(get_dpy(), get_screen());
      LOG::print{INFO}("root = {}", root);
      return root;
    }
}// namespace state

export namespace cyd::ui::graphics {
  struct window_t;
}

export namespace native {
  using namespace cyd::ui::graphics;

  window_t* create_window(
    fabric::async::async_bus_t* async_bus,
    prof::context_t* profiler,
    const char* title,
    const char* wclass,
    int x,
    int y,
    int w,
    int h,
    bool override_redirect = false
  );

  void resize(window_t* win, int w, int h);

  pixelmap_t* get_frame(window_t* win);

  void flush(window_t* win);

  unsigned long get_id(window_t* win);
  std::optional<window_t*> get_from_id(unsigned long id);

  std::pair<int, int> get_position(window_t* win);

  std::pair<int, int> get_size(window_t* win);

  void terminate(window_t* win);
} // namespace cyd::ui::graphics
