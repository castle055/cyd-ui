/*! \file  window.cppm
 *! \brief 
 *!
 */

module;
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>


export module cydui.graphics.window;

import std;

export import fabric.async;
export import fabric.profiling;
export import fabric.type_aliases;


export namespace cyd::ui::graphics {
  struct window_t {
    window_t(
      fabric::async::async_bus_t* async_bus,
      prof::context_t*                 profiler,
      unsigned long                    w,
      unsigned long                    h
    ) {
      this->bus      = async_bus;
      this->profiler = profiler;
    }

    Uint32 window_id() const {
      return SDL_GetWindowID(window);
    }

    fabric::async::async_bus_t* bus;
    prof::context_t* profiler;

    // SDL
    SDL_Window* window     = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Texture* texture   = nullptr;
  };
}
