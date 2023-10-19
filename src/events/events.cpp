//
// Created by castle on 8/21/22.
//

#include "events/events.hpp"
#include "cydstd/logging.hpp"

#include <deque>
#include <list>
#include <mutex>
#include <unordered_map>

// EVENT THREAD IMPLEMENTATION

logging::logger log_task =
  {.name = "EV_TASK", .on = true, .min_level = logging::INFO};
logging::logger log_ctrl =
  {.name = "EV_CTRL", .on = false};

cydui::threading::thread_t* event_thread;
struct thread_data {
  bool pending = false;
  std::deque<cydui::events::Event*>* event_queue =
    new std::deque<cydui::events::Event*>;
  std::unordered_map<
    str,
    std::deque<cydui::events::listener_t>
  >* event_listeners =
    new std::unordered_map<str, std::deque<cydui::events::listener_t>>;
  
  std::unordered_map<str, cydui::events::Event*>* state_events =
    new std::unordered_map<str, cydui::events::Event*>;
};
thread_data* th_data = new thread_data;


std::mutex event_mutex;
std::mutex listeners_mutex;


static void run_event(thread_data* data, cydui::events::Event* ev) {
  ev->ev_mtx.lock();
  ev->status = cydui::events::PROCESSING;
  ev->ev_mtx.unlock();
  listeners_mutex.lock();
  if (data->event_listeners->contains(ev->type)) {
    for (const auto &listener: (*data->event_listeners)[ev->type]) {
      listener(ev);
    }
  }
  listeners_mutex.unlock();
}

void push_event(thread_data* data, cydui::events::Event* ev) {
  event_mutex.lock();
  data->event_queue->push_back(ev);
  data->pending = true;
  log_task.debug("NEW EVENT: %s", ev->type.c_str());
  event_mutex.unlock();
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

void cydui::events::emit_raw(cydui::events::Event* ev) {
  push_event(th_data, ev);
}

void cydui::events::emit_raw(const str &event_type, void* data) {
  auto ev = new cydui::events::Event {
    .type = event_type,
    .ev = data,
  };
  
  push_event(th_data, ev);
}

cydui::events::listener_t cydui::events::on_event_raw(const str &event_type, const Listener &c) {
  if (!th_data->event_listeners->contains(event_type))
    (*th_data->event_listeners).insert({event_type, {}});
  listener_t l(c);
  (*th_data->event_listeners)[event_type].push_back(l);
  return l;
}

void cydui::events::remove_listener(const str &event_type, const listener_t &listener) {
  if (listener.get_id() == 0) return;
  if (th_data->event_listeners->contains(event_type)) {
    for (auto l = (*th_data->event_listeners)[event_type].begin(); l != (*th_data->event_listeners)[event_type].end(); l++) {
      if (l->get_id() == listener.get_id()) {
        (*th_data->event_listeners)[event_type].erase(l);
        delete (u8*) listener.get_id();
      }
    }
  }
}

