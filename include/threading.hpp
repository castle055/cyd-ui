// Copyright (c) 2024, Victor Castillo, All rights reserved.

#ifndef CYD_UI_THREADING_HPP
#define CYD_UI_THREADING_HPP

#include <thread>
#include <chrono>
#include "cydstd/cydstd.h"

namespace cydui::threading {
    struct thread_t {
      void* native_thread = nullptr;
      bool running = false;
      void* data = nullptr;
      
      thread_t* set_name(const str &name);
      
      void join() const;
    };
    
    thread_t* new_thread(void(task)(thread_t* this_thread));
    
    thread_t* new_thread(void(task)(thread_t* this_thread), void* data);
}// namespace cydui::threading

#endif//CYD_UI_THREADING_HPP
