//
// Created by castle on 8/22/22.
//

#include "../../include/threading.hpp"
#include "../../include/logging.hpp"
#include <thread>

using namespace cydui::threading;

logging::logger thlog = {.name = "THREADS"};

thread_t* cydui::threading::new_thread(void(task)(thread_t* this_thread)) {
  return new_thread(task, nullptr);
}

thread_t* cydui::threading::new_thread(
  void(task)(thread_t* this_thread), void* data
) {
  auto* arg = new thread_t();
  arg->running = true;
  arg->data    = data;
  auto* thread = new std::thread(task, arg);
  arg->native_thread = thread;
  return arg;
}


thread_t* cydui::threading::thread_t::set_name(const std::string &name) {
  pthread_t pt = ((std::thread*)native_thread)->native_handle();
  pthread_setname_np(pt, name.c_str());
  return this;
}