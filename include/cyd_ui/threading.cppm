// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include <cyd_fabric/type_aliases.h>

export module cydui.threading;

import std;

using namespace cyd::fabric::type_aliases;

export
namespace cyd::ui::threading {
  struct thread_t {
    void* native_thread = nullptr;
    bool  running       = false;
    void* data          = nullptr;

    thread_t* set_name(const str& name) {
      pthread_t pt = ((std::thread*)native_thread)->native_handle();
      pthread_setname_np(pt, name.c_str());
      return this;
    }

    void join() const {
      ((std::thread*)native_thread)->join();
    }
  };

  thread_t* new_thread(void (task)(thread_t* this_thread), void* data) {
    auto* arg          = new thread_t();
    arg->running       = true;
    arg->data          = data;
    auto* thread       = new std::thread(task, arg);
    arg->native_thread = thread;
    return arg;
  }

  thread_t* new_thread(void (task)(thread_t* this_thread)) {
    return new_thread(task, nullptr);
  }
}// namespace cyd::ui::threading
