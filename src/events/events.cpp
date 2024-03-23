//
// Created by castle on 8/21/22.
//

#include "events/events.hpp"
#include "cydstd/logging.hpp"

#include <deque>
#include <list>
#include <mutex>
#include <unordered_map>
#include <utility>

// EVENT THREAD IMPLEMENTATION

logging::logger log_task =
  {.name = "EV_TASK", .on = true, .min_level = logging::INFO};
logging::logger log_ctrl =
  {.name = "EV_CTRL", .on = true, .min_level = logging::ERROR};

cydui::threading::thread_t* event_thread;
struct thread_data {
  bool pending = false;
  std::deque<cydui::events::Event*>* event_queue =
    new std::deque<cydui::events::Event*>;
  std::unordered_map<
    str,
    std::deque<cydui::events::listener_t*>
  >* event_listeners =
    new std::unordered_map<str, std::deque<cydui::events::listener_t*>>;
  
  std::unordered_map<str, cydui::events::Event*>* state_events =
    new std::unordered_map<str, cydui::events::Event*>;
};
static thread_data* th_data = new thread_data;


std::mutex event_mutex;
std::mutex listeners_mutex;


static void run_event(thread_data* data, cydui::events::Event* ev) {
  ev->ev_mtx.lock();
  ev->status = cydui::events::PROCESSING;
  ev->ev_mtx.unlock();
  listeners_mutex.lock();
  if (data->event_listeners->contains(ev->type)) {
    auto size_before = (*data->event_listeners)[ev->type].size();
    for (const auto listener: (*data->event_listeners)[ev->type]) {
      listener->operator()(ev);
      //! Geniusly removing the insane edge case where a listener modifies
      //! the list of listeners for the SAME EVENT TYPE as it runs.
      //! Even more geniusly introduced the case where that problem is solved
      //! EXCEPT for when a listener adds AND removes the same amount of listeners.
      //!
      //! NOTE that the problem is modifying the list of listeners as it's being
      //! iterated on! This below is a crude way of checking if the list was changed
      if ((*data->event_listeners)[ev->type].size() != size_before) {
        break;
      }
    }
  }
  listeners_mutex.unlock();
}

cydui::events::Event* get_next_event(thread_data* data) {
  cydui::events::Event* ev = nullptr;
  
  //event_mutex.lock();
  if (data->pending) {
    //    log_task.info("EV QUEUE: %d", data->event_queue->size());
    ev = data->event_queue->front();
  }
  //event_mutex.unlock();
  
  return ev;
}

void clean_up_event(thread_data* data, cydui::events::Event* ev) {
  event_mutex.lock();
  data->event_queue->pop_front();
  if (data->event_queue->empty())
    data->pending = false;
  ev->ev_mtx.lock();
  ev->status = cydui::events::CONSUMED;
  ev->ev_mtx.unlock();
  log_task.debug("DONE WITH EVENT: %s %s", ev->type.c_str(), ev->managed ? "" : "[deleting]");
  if (!ev->managed) delete ev;
  event_mutex.unlock();
}

void process_event(thread_data* data) {
  //    log_task.debug("Processing next event");
  
  log_task.debug("EVENT COUNT: %ld", data->event_queue->size());
  cydui::events::Event* ev = get_next_event(data);
  
  if (ev != nullptr) {
    run_event(data, ev);
    clean_up_event(data, ev);
  }
}

using namespace std::chrono_literals;

void event_task(cydui::threading::thread_t* this_thread) {
  log_task.debug("Started event_task");
  auto t0 = std::chrono::system_clock::now();
  while (this_thread->running) {
    t0 = std::chrono::system_clock::now();
    process_event((thread_data*) (this_thread->data));
    std::this_thread::sleep_until(t0 + 100us);
  }
}

// EVENTS API
void cydui::events::start() {
  if (event_thread && event_thread->native_thread != nullptr)
    return;
  log_ctrl.debug("Starting event_thread");
  
  event_thread =
    threading::new_thread(&event_task, th_data)
      ->set_name("EV_THD");
}


void cydui::events::listener_t::remove() {
  if (nullptr != event_queue) event_queue->remove_listener(*this);
  delete (u8*) ID;
  delete func;
  func = nullptr;
  ID = 0;
  active = false;
}
