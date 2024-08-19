/*! \file  api.cppm
 *! \brief 
 *!
 */

export module cydui.graphics:api;

import std;

export import fabric.async;
export import fabric.profiling;

import cydui.graphics.native;

export namespace cyd::ui::graphics {
  using cyd::ui::graphics::window_t;

  window_t* create_window(
    cyd::fabric::async::async_bus_t* async_bus,
    prof::context_t* profiler,
    const char* title,
    const char* wclass,
    int x,
    int y,
    int w,
    int h,
    bool override_redirect = false
  ) {
    return native::create_window(async_bus, profiler, title, wclass, x, y, w, h, override_redirect);
  }

  void resize(window_t* win, int w, int h) {
    native::resize(win, w, h);
  }

  pixelmap_t* get_frame(window_t* win) {
    return native::get_frame(win);
  }

  void flush(window_t* win) {
    native::flush(win);
  }

  unsigned long get_id(window_t* win) {
    return native::get_id(win);
  }
  std::optional<window_t*> get_from_id(unsigned long id) {
    return native::get_from_id(id);
  }

  std::pair<int, int> get_position(window_t* win) {
    return native::get_position(win);
  }

  std::pair<int, int> get_size(window_t* win) {
    return native::get_size(win);
  }

  void terminate(window_t* win) {
    native::terminate(win);
  }
} // namespace cyd::ui::graphics
