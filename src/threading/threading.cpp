// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

#include "cyd_ui/threading.hpp"
#include <thread>
#include <deque>

using namespace cyd::ui::threading;
using namespace std::chrono_literals;

thread_t* cyd::ui::threading::new_thread(void(task)(thread_t* this_thread)) {
  return new_thread(task, nullptr);
}

thread_t* cyd::ui::threading::new_thread(
  void(task)(thread_t* this_thread), void* data
) {
  auto* arg = new thread_t();
  arg->running = true;
  arg->data = data;
  auto* thread = new std::thread(task, arg);
  arg->native_thread = thread;
  return arg;
}


thread_t* cyd::ui::threading::thread_t::set_name(const str &name) {
  pthread_t pt = ((std::thread*) native_thread)->native_handle();
  pthread_setname_np(pt, name.c_str());
  return this;
}

void cyd::ui::threading::thread_t::join() const {
  ((std::thread*) native_thread)->join();
}
