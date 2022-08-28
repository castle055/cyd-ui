//
// Created by castle on 8/22/22.
//

#include "threading.h"
#include "../logging/logging.h"
#include <thread>

using namespace cydui::threading;

logging::logger thlog = { .name = "THREADS" };

thread_t* cydui::threading::new_thread(void (task)(thread_t* this_thread)) {
  new_thread(task, nullptr);
}
thread_t* cydui::threading::new_thread(void (task)(thread_t* this_thread), void* data) {
  auto* arg = new thread_t();
  arg->running = true;
  arg->data = data;
  auto* thread = new std::thread(task, arg);
  arg->native_thread = thread;
  return arg;
}
