// Copyright (c) 2024, Victor Castillo, All rights reserved.

#ifndef CYD_UI_WINDOW_H
#define CYD_UI_WINDOW_H

#include "graphics/graphics.hpp"
#include <cyd_fabric/profiling/profiling.h>

namespace cyd::ui::layout {
  class Layout;
}

namespace cyd::ui::window {
  class CWindow: public cyd::fabric::async::async_bus_t {
  public:
    graphics::window_t* win_ref;
    layout::Layout* layout;

    prof::context_t profiling_ctx{};

    void terminate();

    std::pair<int, int> get_position() const;
    std::pair<int, int> get_size() const;
  };

  CWindow* create(
    layout::Layout* layout,
    const char* title = "CydUI",
    const char* wclass = "cydui",
    int x = 0,
    int y = 0,
    int w = 640,
    int h = 480,
    bool override_redirect = false
  );
}

#endif //CYD_UI_WINDOW_H
