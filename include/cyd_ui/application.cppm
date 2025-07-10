// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include <cyd_fabric_modules/headers/macros/async_events.h>
#include <tracy/Tracy.hpp>
#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

export module cydui.application;

export import std;

export import fabric.async;
export import fabric.logging;

export import cydui.backends;

export namespace cydui {
  EVENT(StopApplicationEvent){};
  
  class Application: public fabric::async::async_bus_t {
  private:
    Application()
        : stop_application_listener_(on_event([&](const StopApplicationEvent&) -> fabric::task<> {
            for (const auto& [id, bus]: this->window_map_) {
              bus->emit<fabric::async::StopBusEvent>();
            }
            this->emit<fabric::async::StopBusEvent>();
            this->stop_application_flag_.test_and_set();
            this->stop_application_flag_.notify_all();
            co_return;
          })) {
      ZoneScopedN("Application{}");
      using namespace std::chrono_literals;
      LOG::print{INFO}("Initializing application...");

      get_executor()
        ->schedule([] -> fabric::task<> {
          tracy::SetThreadNameWithHint("Application", 1);
          ZoneScopedN("Application Init");
          co_return;
        })
        .wait();

      LOG::print{INFO}("Application initialized...");
    }

  public:
    ~Application() {
      ZoneScopedN("~Application");
    }

  public:
    static Application& get_instance() {
      static Application instance;
      return instance;
    }

    static void init() {
      auto& app = get_instance();
    }

    template <typename... Args>
    static auto run_async(
      auto&& fun,
      Args&&... args
    ) {
      ZoneScopedN("Application:run_async");
      return get_instance()->schedule(
        [=](Args... argss) -> fabric::task<decltype(fun(std::forward<Args>(argss)...))> {
          ZoneScopedN("Application:run_async:()");
          try {
            if constexpr (std::is_void_v<decltype(fun(std::forward<Args>(argss)...))>) {
              fun(std::forward<Args>(argss)...);
              co_return;
            } else {
              co_return fun(std::forward<Args>(argss)...);
            }
          } catch (std::exception& e) {
            LOG::print{FATAL}("{}", e.what());
            throw;
          }
        },
        std::forward<Args>(args)...
      );
    }

    template <typename... Args>
    static auto run(
      auto&& fun,
      Args&&... args
    ) {
      ZoneScopedN("Application:run");
      if constexpr (std::is_void_v<decltype(fun(std::forward<Args>(args)...))>) {
        run_async(fun, std::forward<Args>(args)...).get();
      } else {
        return run_async(fun, std::forward<Args>(args)...).get();
      }
    }

    template <typename... Args>
    static auto schedule(
      auto&& fun,
      Args&&... args
    ) {
      ZoneScopedN("Application:run_async");
      return get_instance()->schedule(fun, std::forward<Args>(args)...);
    }

    template <typename... Args>
    static auto schedule(
      fabric::tasks::time_point tp,
      auto&&                    fun,
      Args&&... args
    ) {
      ZoneScopedN("Application:run_async");
      return get_instance()->schedule(tp, fun, std::forward<Args>(args)...);
    }

    template <typename... Args>
    static auto schedule(
      fabric::tasks::duration duration,
      auto&&                  fun,
      Args&&... args
    ) {
      ZoneScopedN("Application:run_async");
      return get_instance()->schedule(duration, fun, std::forward<Args>(args)...);
    }

    static void register_window(
      std::size_t                 id,
      fabric::async::async_bus_t* window
    ) {
      ZoneScopedN("Application:register_window");
      std::scoped_lock lk{get_instance().window_map_mtx_};
      get_instance().window_map_[id] = window;
      LOG::print{INFO}("Window registered ID={}", id);
    }
    static void unregister_window(std::size_t id) {
      ZoneScopedN("Application:unregister_window");
      Application&     instance = get_instance();
      std::scoped_lock lk{instance.window_map_mtx_};
      instance.window_map_.erase(id);
      if (instance.window_map_.empty() and instance.stop_application_flag_.test()) {
        instance.stop_application_flag_.clear();
        instance.emit<fabric::async::StopBusEvent>();
      }
      LOG::print{INFO}("Window unregistered ID={}", id);
    }

    template <
      backends::BackendConcept BackendType,
      typename... Args>
    static std::shared_ptr<BackendType> init_backend(Args&&... args) {
      ZoneScopedN("Application:init_backend");
      Application&    instance   = get_instance();
      refl::type_id_t backend_id = refl::type_id<BackendType>;
      if (instance.loaded_backends_.contains(backend_id)) {
        throw std::runtime_error("Backend already initialized");
      }

      std::shared_ptr<BackendType> backend =
        backends::make_backend<BackendType>(std::forward<Args>(args)...);
      instance.loaded_backends_[backend_id] = backend;
      return backend;
    }

    template <backends::BackendConcept BackendType>
    static void deinit_backend() {
      ZoneScopedN("Application:deinit_backend");
      Application&    instance   = get_instance();
      refl::type_id_t backend_id = refl::type_id<BackendType>;
      if (not instance.loaded_backends_.contains(backend_id)) {
        return;
      }
      instance.loaded_backends_.erase(backend_id);
    }

    template <backends::BackendConcept BackendType>
    static std::shared_ptr<BackendType> get_backend() {
      ZoneScopedN("Application:get_backend");
      Application&    instance   = get_instance();
      refl::type_id_t backend_id = refl::type_id<BackendType>;
      if (not instance.loaded_backends_.contains(backend_id)) {
        throw std::runtime_error("Backend already initialized");
      }
      return std::dynamic_pointer_cast<BackendType>(instance.loaded_backends_.at(backend_id));
    }


    static void terminate() {
      get_instance()->request_stop();
    }

  private:
    TracyLockable(
      std::mutex,
      window_map_mtx_
    );
    std::map<std::size_t, fabric::async::async_bus_t*>                window_map_{};
    fabric::async::listener<StopApplicationEvent>                     stop_application_listener_;
    std::atomic_flag                                                  stop_application_flag_{false};
    std::unordered_map<refl::type_id_t, backends::backend_base::sptr> loaded_backends_{};
  };

  void init() {
    ZoneScopedN("Application:init");
    Application::init();
  }

  template <
    backends::BackendConcept BackendType,
    typename... Args>
  std::shared_ptr<BackendType> init_backend(Args&&... args) {
    ZoneScopedN("Application:init_backend");
    auto t = Application::schedule([&]() -> fabric::task<std::shared_ptr<BackendType>> {
      co_return Application::init_backend<BackendType>(std::forward<Args>(args)...);
    });
    return t.get();
  }

  template <backends::BackendConcept BackendType>
  void deinit_backend() {
    ZoneScopedN("Application:init_backend");
    auto t = Application::schedule([]() -> fabric::task<> {
      Application::deinit_backend<BackendType>();
      co_return;
    });
    t.wait();
  }

  template <
    backends::BackendConcept BackendType,
    typename... Args>
  backends::FrameSptrConcept auto make_frame(Args&&... args) {
    auto t = Application::schedule([&]() -> fabric::task<typename BackendType::frame_type::sptr> {
      auto backend = Application::get_backend<BackendType>();
      co_return std::dynamic_pointer_cast<typename BackendType::frame_type>(
        backend->make_frame(std::forward<Args>(args)...)
      );
    });
    return t.get();
  }
} // namespace cydui

// using cyd::fabric::async::async;
