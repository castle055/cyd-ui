//
// Created by castle on 4/1/24.
//

#ifndef CYD_UI_EVENT_QUEUE_H
#define CYD_UI_EVENT_QUEUE_H

#include <utility>
#include <queue>

#include "events.h"

namespace cydui::async {
    class event_queue_t {
      logging::logger log_task = {.name = "EV_Q", .on = true, .min_level = logging::INFO};
    private:
      std::queue<event_t> front_event_queue {};
      std::queue<event_t> back_event_queue {};
      
      std::unordered_map<
        str,
        std::deque<cydui::async::listener_t*>
      > event_listeners {};
      
      std::unordered_map<str, cydui::async::event_t>* state_events =
        new std::unordered_map<str, cydui::async::event_t>;
    private:
      std::mutex event_mutex;
      std::mutex listeners_mutex;
    
    private: // * Private functions
      // ? This function creates a copy of the eve, thus increasing its ref count.
      void push_event(const cydui::async::event_t &ev) {
        {
          std::scoped_lock lk {event_mutex};
          front_event_queue.push(ev);
        }
        log_task.debug("NEW EVENT: %s", ev->type.c_str());
      }
    
    private: // * Raw Event Handling
      event_t &emit_raw(event_t &ev) {
        push_event(ev);
        return ev;
      }
      
      event_t emit_raw(const str &event_type, void* data, std::function<void()> &&data_destructor) {
        event_t ev = make_event(event_type, data, std::forward<std::function<void()> &&>(data_destructor));
        push_event(ev);
        return ev;
      }
    
    private:
      void swap_event_queues() {
        std::scoped_lock lk {event_mutex};
        std::swap(front_event_queue, back_event_queue);
      }
      void process_event(const event_t &ev) {
        ev->status = PROCESSING;
        std::vector<listener_t*> listeners {};
        
        { // Make copy of listeners with the mutex
          std::scoped_lock lk {listeners_mutex};
          if (event_listeners.contains(ev->type)) {
            std::deque<listener_t*> &ev_listeners = event_listeners[ev->type];
            listeners.reserve(ev_listeners.size());
            for (const auto &item: ev_listeners) {
              listeners.push_back(item);
            }
          }
        }
        
        // Iterate over copy of listeners list. This should allow any listener to modify the listeners list (ie: removing themselves)
        for (auto &listener: listeners) {
          listener->operator()(ev);
        }
        
        // Clean up event
        ev->status = CONSUMED;
      }
      void process_all_events(std::queue<event_t> &ev_queue) {
        event_t ev;
        while (!ev_queue.empty()) {
          ev = ev_queue.front();
          ev_queue.pop();
          process_event(ev);
        }
      }
    protected: // * Bus Interface
      void events_process_batch() {
        swap_event_queues();
        process_all_events(back_event_queue);
      }
    public: // * Public Interface
      template<typename T>
      requires EventType<T>
      inline event_t emit(typename T::DataType data) {
        auto* data_ptr = new T({.data = data});
        return emit_raw(T::type, data_ptr, [data_ptr]() {delete data_ptr;});
      }
      
      listener_t* on_event_raw(const str &event_type, const Listener &l_) {
        if (!event_listeners.contains(event_type))
          event_listeners.insert({event_type, {}});
        auto* l = new listener_t {this, event_type, l_};
        event_listeners[event_type].push_back(l);
        return l;
      }
      template<typename T>
      requires EventType<T>
      inline listener_t* on_event(Consumer<T> c) {
        return on_event_raw(T::type, [&, c](event_t &ev) {
          auto parsed = ev->parse<T>();
          if (parsed.data) {
            c(parsed);
          }
        });
      }
      
      void remove_listener(const listener_t &listener) {
        if (listener.get_id() == 0) return;
        const str &event_type = listener.event_type;
        if (event_listeners.contains(event_type)) {
          for (auto l = event_listeners[event_type].begin(); l != event_listeners[event_type].end(); l++) {
            if ((*l)->get_id() == listener.get_id()) {
              event_listeners[event_type].erase(l);
              break;
            }
          }
        }
      }
    
    public: // * Getter
      event_queue_t &get_event_queue() {
        return *this;
      }
    };
}
#endif //CYD_UI_EVENT_QUEUE_H