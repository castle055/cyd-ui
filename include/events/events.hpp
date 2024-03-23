//
// Created by castle on 8/21/22.
//

#ifndef CYD_UI_EVENTS_HPP
#define CYD_UI_EVENTS_HPP


#include <functional>
#include <mutex>
#include <string>
#include <utility>
#include <deque>
#include <unordered_map>
#include <type_traits>

#include "../cydstd/cydstd.h"
#include "../threading.hpp"
#include "cydstd/logging.hpp"

namespace cydui::events {
    template<class T>
    concept EventType = requires(T::DataType dt) {
      {
      T::type
      } -> std::convertible_to<str>;
      typename T::DataType;
      {
      T::data
      } -> std::convertible_to<typename T::DataType>;
    };
    
    template<typename T> requires EventType<T>
    struct ParsedEvent {
      using DataType = T::DataType;
      
      const str type;
      const typename T::DataType* data;
    };
    
    enum EventStatus {
      PENDING,
      PROCESSING,
      CONSUMED,
    };
    
    struct Event {
      str type;
      EventStatus status = PENDING;
      void* ev;
      
      // Unfortunately needed since `ev` can be of any type
      std::function<void()> ev_destructor = [](){};
      ~Event() {
        ev_destructor();
      }
      
      std::mutex ev_mtx;
      
      // If it is someone else's job to delete this object
      bool managed = false;
      
      template<typename T>
      requires EventType<T>
      ParsedEvent<T> parse() {
        str matchName = T::type;
        if (matchName == type) {
          return ParsedEvent<T> {
            .type = type,
            .data = (typename T::DataType*) ev,
          };
        } else {
          return ParsedEvent<T> {
            .type = type,
            .data = nullptr,
          };
        }
      }
    };
    
    template<typename T> requires EventType<T>
    using Consumer = std::function<void(const ParsedEvent<T>&)>;
    
    typedef std::function<void(Event*)> Listener;
    struct listener_t;
    
    class event_queue_t;
    
    class listener_t {
      long ID;
      bool active = true;
    public:
      listener_t(): event_queue(nullptr) {
        ID = 0;
        active = false;
      }
      
      explicit listener_t(event_queue_t* event_queue_, str  type, Listener c)
        : event_queue(event_queue_),
          event_type(std::move(type)),
          func(new Listener {std::move(c)}) {
        ID = (long) new u8;
      }
      //
      //listener_t(const listener_t &other) {
      //  ID = other.get_id();
      //}
      
      [[nodiscard]] long get_id() const {
        return ID;
      }
      
      event_queue_t* const event_queue;
      str event_type;
      Listener* func = nullptr;
      
      void remove();
      
      bool is_active() const {
        return active;
      }
      
      void operator()(Event* ev) const {
        if (nullptr != func) {
          func->operator()(ev);
        }
      }
    };
    
    void start();
  
    class event_queue_t {
      logging::logger log_task = {.name = "EV_Q", .on = true, .min_level = logging::INFO};
    private:
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
    private:
      std::mutex event_mutex;
      std::mutex listeners_mutex;
    
    private: // * Private functions
      void push_event(cydui::events::Event* ev) {
        event_mutex.lock();
        event_queue->push_back(ev);
        pending = true;
        log_task.debug("NEW EVENT: %s", ev->type.c_str());
        event_mutex.unlock();
      }

    private: // * Raw Event Handling
      void emit_raw(cydui::events::Event* ev) {
        push_event(ev);
      }
      
      void emit_raw(const str &event_type, void* data, std::function<void()> data_destructor){
        auto* ev = new cydui::events::Event {
          .type = event_type,
          .ev = data,
          .ev_destructor = std::move(data_destructor),
        };
        
        push_event(ev);
      }
      
      listener_t* on_event_raw(const str &event_type, const Listener &l_) {
        if (!event_listeners->contains(event_type))
          (*event_listeners).insert({event_type, {}});
        auto* l = new listener_t {this, event_type, l_};
        (*event_listeners)[event_type].push_back(l);
        return l;
      }
    protected: // * Bus Interface
    public: // * Public Interface
      template<typename T>
      requires EventType<T>
      inline void emit(typename T::DataType data) {
        auto* data_ptr = new T({.data = data});
        emit_raw(T::type, data_ptr, [data_ptr](){ delete data_ptr; });
      }
      
      template<typename T>
      requires EventType<T>
      inline listener_t* on_event(Consumer<T> c) {
        return on_event_raw(T::type, [&,c](Event* ev) {
          auto parsed = ev->parse<T>();
          if (parsed.data) {
            c(parsed);
          }
        });
      }
      
      void remove_listener(const listener_t &listener) {
        if (listener.get_id() == 0) return;
        const str &event_type = listener.event_type;
        if (event_listeners->contains(event_type)) {
          for (auto l = (*event_listeners)[event_type].begin(); l != (*event_listeners)[event_type].end(); l++) {
            if ((*l)->get_id() == listener.get_id()) {
              (*event_listeners)[event_type].erase(l);
              break;
            }
          }
        }
      }
    
    public: // * Getter
      event_queue_t& get_event_queue() {
        return *this;
      }
    };
    // win.emit<RedrawEvent>({});
    
    class coroutine_runtime_t {
    
    protected: // * Bus Interface
    public: // * Public Interface
    public: // * Getter
      coroutine_runtime_t& get_coroutine_runtime() {
        return *this;
      }
    };
    
    class scheduler_t {
      event_queue_t& event_queue;
      coroutine_runtime_t& coroutine_runtime;
    protected: // * Bus Interface
      template <class Bus> requires requires(Bus bus) {
        { bus.get_event_queue() } -> std::convertible_to<event_queue_t&>;
        { bus.get_coroutine_runtime() } -> std::convertible_to<coroutine_runtime_t&>;
      }
      scheduler_t(Bus* bus)
        : event_queue(bus->get_event_queue()),
          coroutine_runtime(bus->get_coroutine_runtime())
          { }
    
    public: // * Public Interface
    public: // * Getter
      scheduler_t& get_scheduler() {
        return *this;
      }
    };
    
    class async_bus_t: public event_queue_t,
                       public coroutine_runtime_t,
                       public scheduler_t {
    public: // Construction & RAII
      // ! Constructor
      async_bus_t(): scheduler_t(this) {}
      // ! Destructor
      ~async_bus_t() {}
      // ! Copy
      async_bus_t(const async_bus_t& rhs) = delete;
      async_bus_t& operator=(const async_bus_t& rhs) = delete;
      // ! Move
      async_bus_t(async_bus_t&& rhs) = delete;
      async_bus_t& operator=(async_bus_t&& rhs) = delete;
    };
}// namespace cydui::events


// MACROS
#define consumer [=](it)
#define listen(EVENT, ...) \
  on_event<EVENT>(cydui::events::Consumer<EVENT>( \
      [&](const cydui::events::ParsedEvent<EVENT>& ev) __VA_ARGS__))


struct event_data_type_base_t {
  unsigned long win = 0;
};
// TODO - In case of event name collision (it's possible), maybe append __FILE__ or __LINE__ to the `type`
// variable to make the event type unique even if multiple translation units declare similarly named events.
#define EVENT(NAME, DATA)                                                      \
  struct NAME {                                                                \
    constexpr static const char* type = #NAME;                                 \
    struct DataType: public event_data_type_base_t DATA data;                  \
  };


#endif//CYD_UI_EVENTS_HPP
