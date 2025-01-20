/*! \file  window.cppm
 *! \brief 
 *!
 */

module;
#include <cyd_fabric_modules/headers/macros/async_events.h>
#include <tracy/Tracy.hpp>
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

export module cydui:window;

import std;
import fabric.logging;
import fabric.async;
import fabric.profiling;

export import cydui.graphics;
export import cydui.window_events;

export import cydui.application;

export namespace cyd::ui {
  class CWindow;
  class Layout;
}

namespace cyd::ui {
  void bind_layout(cyd::ui::Layout* layout, const std::shared_ptr<cyd::ui::CWindow> &window);
}

export namespace cyd::ui {
  class CWindow: public fabric::async::async_bus_t {
    CWindow(
      Layout* layout,
      std::string title,
      int x,
      int y,
      int width,
      int height
    );

  public:
    using sptr = std::shared_ptr<CWindow>;

    ~CWindow();

    struct builder_t {
      builder_t(Layout* layout): layout_(layout) {
      }

      builder_t(const builder_t &) = delete;

      builder_t &position(int x, int y) {
        x_ = x;
        y_ = y;
        return *this;
      }

      builder_t &size(int width, int height) {
        width_  = width;
        height_ = height;
        return *this;
      }

      builder_t &title(const std::string &title) {
        title_ = title;
        return *this;
      }

      sptr show() const {
        auto ptr = std::shared_ptr<CWindow>(new CWindow(layout_, title_, x_, y_, width_, height_));
        ptr->coroutine_enqueue([](Layout* lyt, sptr win) -> fabric::async::async<bool> {
          bind_layout(lyt, win);
          LOG::print {INFO}("Layout bound to window");
          co_return true;
        }, layout_, ptr);
        ptr->start();
        return ptr;
      }

    private:
      Layout* layout_    = nullptr;
      int x_             = SDL_WINDOWPOS_UNDEFINED;
      int y_             = SDL_WINDOWPOS_UNDEFINED;
      int width_         = 1280;
      int height_        = 720;
      std::string title_ = "CYD-UI";
    };

    static builder_t make(Layout* layout) {
      return builder_t(layout);
    }

    template<typename Component>
    static builder_t make(typename Component::props_t props = { });

    graphics::window_t* native();

    std::unique_ptr<graphics::window_t> win_ref;
    Layout* layout;

    prof::context_t profiling_ctx { };

    void terminate();

    bool is_open() const;

    std::pair<int, int> get_position();

    std::pair<int, int> get_size();

    template <typename... Args>
    void run(auto&& fun, Args&&... args) {
      std::latch completion_latch {1};
      this->coroutine_enqueue([&](Args... argss) -> fabric::async::async<bool> {
        fun(std::forward<Args>(argss)...);
        completion_latch.count_down();
        co_return true;
      }, std::forward<Args>(args)...);
      completion_latch.wait();
    }

    template <typename... Args>
    void run_async(auto&& fun, Args&&... args) {
      this->coroutine_enqueue([&](Args... argss) -> fabric::async::async<bool> {
        fun(std::forward<Args>(argss)...);
        co_return true;
      }, std::forward<Args>(args)...);
    }


    compositing::LayoutCompositor compositor { };
  };

  //* Implementation

  CWindow::CWindow(
    Layout* layout,
    std::string title,
    int x,
    int y,
    int width,
    int height
  ): win_ref(std::make_unique<graphics::window_t>(this, nullptr, width, height)),
     layout(layout) {
    add_init([=,this] {
      Application::run([=](CWindow* self) {
        self->win_ref->window = SDL_CreateWindow(
          title.c_str(),
          SDL_WINDOWPOS_UNDEFINED,
          SDL_WINDOWPOS_UNDEFINED,
          width,
          height,
          SDL_WINDOW_RESIZABLE
        );
        SDL_SetWindowPosition(self->win_ref->window, x, y);
        SDL_SetWindowSize(self->win_ref->window, width, height);
        // SDL_FlashWindow(win_ref->window, SDL_FLASH_UNTIL_FOCUSED);

        Application::register_window(self->win_ref->window_id(), self);

        self->win_ref->renderer = SDL_CreateRenderer(self->win_ref->window, -1, SDL_RENDERER_ACCELERATED);

        self->compositor.set_render_target(self->win_ref.get(), &self->profiling_ctx);
        LOG::print {INFO}("SDL2 Window initialized.");
      }, this);
    });
  }

  CWindow::~CWindow() {
    Application::unregister_window(win_ref->window_id());
    this->stop();
  }

  graphics::window_t* CWindow::native() {
    return win_ref.get();
  }

  void CWindow::terminate() {
    // TODO - Implement
    // delete layout;
    // graphics::terminate(win_ref);
    // win_ref = nullptr;
  }

  bool CWindow::is_open() const {
    return win_ref != nullptr;
  }

  std::pair<int, int> CWindow::get_position() {
    int x, y;
    Application::run([](int* x, int* y, CWindow* self) {
      SDL_GetWindowPosition(self->win_ref->window, x, y);
    }, &x, &y, this);
    return {x, y};
  }

  std::pair<int, int> CWindow::get_size() {
    int w, h;
    Application::run([](int* w, int* h, CWindow* self) {
      SDL_GetRendererOutputSize(self->win_ref->renderer, w, h);
    }, &w, &h, this);
    return {w, h};
  }
}
