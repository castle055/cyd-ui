// Copyright (c) 2024, Victor Castillo, All rights reserved.

#ifndef CYD_UI_ASYNC_BUS_H
#define CYD_UI_ASYNC_BUS_H

#include "event_queue.h"
#include "coroutine_runtime.h"

namespace cydui::async {
    enum class async_bus_status_e {
      RUNNING,
      STOPPED,
    };
    
    class async_bus_t: public event_queue_t,
                       public coroutine_runtime_t {
    public: /// @name Construction & RAII
      // ! Constructor
      async_bus_t() {
        thread_start();
      }
      // ! Destructor
      ~async_bus_t() {
        thread_stop();
      }
      // ! Copy
      async_bus_t(const async_bus_t &rhs) = delete;
      async_bus_t &operator=(const async_bus_t &rhs) = delete;
      // ! Move
      async_bus_t(async_bus_t &&rhs) = delete;
      async_bus_t &operator=(async_bus_t &&rhs) = delete;
    
    private TEST_PUBLIC: /// @name Status
      std::atomic<async_bus_status_e> status = async_bus_status_e::STOPPED;
    private TEST_PUBLIC: /// @name Thread
      std::unique_ptr<std::thread> thread = nullptr;
      void thread_start() {
        if (!thread) {
          status = async_bus_status_e::RUNNING;
          thread = std::make_unique<std::thread>([](async_bus_t* bus) {bus->thread_task();}, this);
        }
      }
      void thread_stop() {
        if (thread) {
          status = async_bus_status_e::STOPPED;
          thread->join();
          thread.reset(nullptr);
        }
      }
      
      void thread_task() {
        using namespace std::chrono_literals;
        auto prev_t = std::chrono::system_clock::now();
        while (status == async_bus_status_e::RUNNING) {
          prev_t = std::chrono::system_clock::now();
          events_process_batch();
          coroutine_run();
          std::this_thread::sleep_until(prev_t + 100us);
        }
      }
    };
}

#endif //CYD_UI_ASYNC_BUS_H
