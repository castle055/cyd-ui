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

// EVENT THREAD IMPLEMENTATION

logging::logger log_task =
        {.name = "EV_TASK", .on = false};
logging::logger log_ctrl =
        {.name = "EV_CTRL", .on = false};

cydui::threading::thread_t* event_thread;
struct thread_data {
    std::deque<cydui::events::Event*>* event_queue =
            new std::deque<cydui::events::Event*>;
    std::unordered_map<
            std::string,
            std::deque<cydui::events::Listener>
    >* event_listeners =
            new std::unordered_map<std::string, std::deque<cydui::events::Listener>>;

    std::unordered_map<std::string, cydui::events::Event*>* state_events =
            new std::unordered_map<std::string, cydui::events::Event*>;
};
thread_data* th_data = nullptr;


std::mutex event_mutex;
std::mutex listeners_mutex;


static void run_event(thread_data* data, cydui::events::Event* ev) {
    ev->status = cydui::events::PROCESSING;
    listeners_mutex.lock();
    if (data->event_listeners->contains(ev->type)) {
        for (const auto &listener: (*data->event_listeners)[ev->type]) {
            listener(ev);
        }
    }
    listeners_mutex.unlock();
    ev->status = cydui::events::CONSUMED;
}

void push_event(thread_data* data, cydui::events::Event* ev) {
    if (!event_mutex.try_lock()) return;
    data->event_queue->push_back(ev);
    event_mutex.unlock();
}

cydui::events::Event* get_next_event(thread_data* data) {
    cydui::events::Event* ev = nullptr;

    if (!event_mutex.try_lock()) return nullptr;
    if (!data->event_queue->empty()) {
        ev = data->event_queue->front();
    }
    event_mutex.unlock();

    return ev;
}

void clean_up_event(thread_data* data, cydui::events::Event* ev) {
    if (!event_mutex.try_lock()) return;
    data->event_queue->pop_front();
    delete ev;
    event_mutex.unlock();
}

void process_event(thread_data* data) {
    log_task.debug("Processing next event");

    cydui::events::Event* ev = get_next_event(data);

    if (ev != nullptr) {
        run_event(data, ev);
        clean_up_event(data, ev);
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

// EVENTS API
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

void cydui::events::emit_raw(const std::string &event_type, void* data) {
  auto ev = new cydui::events::Event {
    .type = event_type,
    .ev = data,
  };
  
  push_event(th_data, ev);
}

void cydui::events::on_event_raw(const std::string &event_type, const Listener &c) {
  if (!th_data->event_listeners->contains(event_type))
    (*th_data->event_listeners)[event_type] = { };
  (*th_data->event_listeners)[event_type].push_back(c);
}


