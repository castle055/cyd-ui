//
// Created by castle on 8/21/22.
//

#include "../../include/events.hpp"
#include "../../include/logging.hpp"
#include "../threading/threading.hpp"

#include <deque>
#include <list>
#include <mutex>
#include <unordered_map>
#include <vector>

std::mutex event_mutex;
std::mutex listeners_mutex;

cydui::threading::thread_t* event_thread;
struct thread_data {
  std::deque<cydui::events::CEvent*>       * event_queue     =
                                             new std::deque<cydui::events::CEvent*>;
  std::list<cydui::events::CEventListener*>* event_listeners =
                                             new std::list<cydui::events::CEventListener*>;
  
  std::unordered_map<std::string, cydui::events::CEvent*>* state_events =
                                                           new std::unordered_map<std::string, cydui::events::CEvent*>;
};

thread_data* th_data = nullptr;

logging::logger                                                                        log_task =
                                                                                         {.name = "EV_TASK", .on = false};
logging::logger                                                                        log_ctrl =
                                                                                         {.name = "EV_CTRL", .on = false};

static void run_event(thread_data* data, cydui::events::CEvent* ev) {
  listeners_mutex.lock();
  for (const auto &listener: *data->event_listeners) {
    listener->on_event(ev);
  }
  listeners_mutex.unlock();
  
  if (ev->consumed)
    delete ev;
}

void process_event(thread_data* data) {
  //  log_task.debug("Processing next event");
  //cydui::events::CEvent* ev = nullptr;
  std::vector<cydui::events::CEvent*> evs;
  std::deque<cydui::events::CEvent*>  * queue = nullptr;
  
  if (!event_mutex.try_lock())
    return;
  //  log_task.debug("Event queue len: %d", data->event_queue->size());
  if (!data->event_queue->empty()) {
    //  log_task.debug("Event queue not empty");
    queue = data->event_queue;
    data->event_queue = new std::deque<cydui::events::CEvent*>;
    log_task.debug("POP Event");
  }
  // TODO - This is a very inefficient implementation
  static unsigned int lap = 0;
  if (lap++ == 0) {
    for (auto &item: *data->state_events) {
      evs.push_back(item.second);
    }
  }
  if (lap > 20) lap = 0;
  //data->state_events->clear();
  event_mutex.unlock();
  
  if (queue) {
    for (auto &e: *queue) {
      run_event(data, e);
    }
    delete queue;
  }
  
  for (auto &e: evs) {
    if (!e->consumed)
      run_event(data, e);
  }
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
  th_data = new thread_data;
  event_thread =
    threading::new_thread(&event_task, th_data)
      ->set_name("EV_THD");
}

void cydui::events::emit_raw(std::string event_type, void* data) {
  new cydui::events::Event {
    .type = event_type,
    .ev = data,
  };
}

std::unordered_map<std::string, std::deque<std::function<void(cydui::events::Event)>>> l;

void cydui::events::on_event(std::string event_type, std::function<void(Event)> c) {
  if (!l.contains(event_type)) l[event_type] = { };
  l[event_type].push_back(c);
}


