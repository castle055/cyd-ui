//
// Created by castle on 8/21/22.
//

#ifndef CYD_UI_EVENTS_HPP
#define CYD_UI_EVENTS_HPP


#include <functional>
#include <mutex>
#include <string>
#include <utility>

#include "cydstd/cydstd.h"

namespace cydui::events {
    template<class T>
    concept EventType = requires {
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
    
    void emit_raw(cydui::events::Event* ev);
    
    void emit_raw(const str &event_type, void* data);
    
    template<typename T>
    requires EventType<T>
    inline void emit(typename T::DataType data) {
      emit_raw(T::type, new T({.data = data}));
    }
    
    template<typename T> requires EventType<T>
    class Consumer: public std::function<void(const ParsedEvent<T> &)> {
    public:
#pragma clang diagnostic push
#pragma ide diagnostic ignored "google-explicit-constructor"
      
      Consumer(std::function<void(const ParsedEvent<T> &)> c)
        : std::function<void(const ParsedEvent<T> &)>(c) {
      }

#pragma clang diagnostic pop
    };
    
    
    typedef std::function<void(Event*)> Listener;
    struct listener_t;
    
    void remove_listener(const str &event_type, const listener_t &listener);
    
    struct listener_t {
    private:
      long ID;
      bool active = true;
    public:
      explicit listener_t(Listener c): func(std::move(c)) {
        ID = (long) new u8;
      }
      //
      //listener_t(const listener_t &other) {
      //  ID = other.get_id();
      //}
      
      [[nodiscard]] long get_id() const {
        return ID;
      }
      
      str event_type;
      Listener func;
      
      void remove() {
        cydui::events::remove_listener(event_type, *this);
        active = false;
      }
      
      bool is_active() const {
        return active;
      }
      
      void operator()(Event* ev) const {
        func(ev);
      }
    };
    
    listener_t on_event_raw(const str &event_type, const Listener &l);
    
    template<typename T>
    requires EventType<T>
    inline listener_t on_event(Consumer<T> c) {
      return cydui::events::on_event_raw(T::type, [c](Event* ev) {
        auto parsed = ev->parse<T>();
        if (parsed.data) {
          c(parsed);
        }
      });
    }
    
    
    void start();
  
}// namespace cydui::events

// MACROS
#define consumer [=](it)
#define listen(EVENT, block)                                                   \
  cydui::events::on_event<EVENT>(cydui::events::Consumer<EVENT>(               \
      [=, this](const cydui::events::ParsedEvent<EVENT>& it) block))

// TODO - In case of event name collision (it's possible), maybe append __FILE__ or __LINE__ to the `type`
// variable to make the event type unique even if multiple translation units declare similarly named events.
#define EVENT(NAME, DATA)                                                      \
  struct NAME {                                                                \
    constexpr static const char* type = #NAME;                                 \
    struct DataType DATA data;                                                 \
  };


#endif//CYD_UI_EVENTS_HPP
