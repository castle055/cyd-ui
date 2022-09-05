//
// Created by castle on 8/21/22.
//

#include "events.hpp"
#include "../logging/logging.hpp"
#include "../threading/threading.hpp"

#include <deque>
#include <list>
#include <mutex>

std::mutex event_mutex;
std::mutex listeners_mutex;

cydui::threading::thread_t* event_thread;
struct thread_data {
  std::deque<cydui::events::CEvent*>       * event_queue     =
                                               new std::deque<cydui::events::CEvent*>;
  std::list<cydui::events::CEventListener*>* event_listeners =
                                               new std::list<cydui::events::CEventListener*>;
};

thread_data* th_data = nullptr;

logging::logger log_task = {.name = "EV_TASK", .on = false};
logging::logger log_ctrl = {.name = "EV_CTRL", .on = false};

void process_event(thread_data* data) {
  //  log_task.debug("Processing next event");
  cydui::events::CEvent* ev;
  
  event_mutex.lock();
  //  log_task.debug("Event queue len: %d", data->event_queue->size());
  if (data->event_queue->empty()) {
    //    log_task.debug("Event queue empty");
    event_mutex.unlock();
    return;
  }
  //  log_task.debug("Event queue not empty");
  ev = data->event_queue->front();
  data->event_queue->pop_front();
  log_task.debug("POP Event");
  event_mutex.unlock();
  
  listeners_mutex.lock();
  for (const auto &listener: *data->event_listeners) {
    listener->on_event(ev);
  }
  listeners_mutex.unlock();
  
  delete ev;
}

using namespace std::chrono_literals;

void event_task(cydui::threading::thread_t* this_thread) {
  log_task.debug("Started event_task");
  while (this_thread->running) {
    process_event((thread_data*)(this_thread->data));
    //std::this_thread::sleep_for(500us);
  }
}

void cydui::events::start() {
  if (event_thread && event_thread->native_thread != nullptr)
    return;
  log_ctrl.debug("Starting event_thread");
  
  delete th_data;
  th_data      = new thread_data;
  event_thread = threading::new_thread(&event_task, th_data);
}

void cydui::events::emit(cydui::events::CEvent* ev) {
  log_ctrl.debug("Emitting event. type=%d", ev->type);
  event_mutex.lock();
  th_data->event_queue->push_back(ev);
  event_mutex.unlock();
}

void cydui::events::subscribe(CEventListener* listener) {
  listeners_mutex.lock();
  th_data->event_listeners->push_back(listener);
  listeners_mutex.unlock();
}

void cydui::events::unsubscribe(CEventListener* listener) {
  listeners_mutex.lock();
  th_data->event_listeners->remove(listener);
  listeners_mutex.unlock();
}
