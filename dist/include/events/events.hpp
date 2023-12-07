//
// Created by castle on 8/21/22.
//

#ifndef CYD_UI_EVENTS_HPP
#define CYD_UI_EVENTS_HPP


#include <functional>
#include <mutex>
#include <string>
#include <utility>

#include "../cydstd/cydstd.h"
#include "../threading.hpp"

//namespace events {
//    template<class T>
//    concept EventType = requires {
//      {
//      T::type
//      } -> std::convertible_to<str>;
//      typename T::DataType;
//      {
//      T::data
//      } -> std::convertible_to<typename T::DataType>;
//    };
//
//    template<typename T> requires EventType<T>
//    struct ParsedEvent {
//      using DataType = T::DataType;
//
//      const str type;
//      const typename T::DataType* data;
//    };
//
//    enum EventStatus {
//      PENDING,
//      PROCESSING,
//      CONSUMED,
//    };
//
//    struct Event {
//      str type;
//      EventStatus status = PENDING;
//      void* ev;
//
//      std::mutex ev_mtx;
//
//      // If it is someone else's job to delete this object
//      bool managed = false;
//
//      template<typename T>
//      requires EventType<T>
//      ParsedEvent<T> parse() {
//        str matchName = T::type;
//        if (matchName == type) {
//          return ParsedEvent<T> {
//            .type = type,
//            .data = (typename T::DataType*) ev,
//          };
//        } else {
//          return ParsedEvent<T> {
//            .type = type,
//            .data = nullptr,
//          };
//        }
//      }
//    };
//
//    struct event_emitter_t {
//      virtual void emit() = 0;
//    };
//
//    class event_bus_t {
//
//    };
//}

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
      //{
      //dt.win
      //} -> std::convertible_to<unsigned long>;
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
    
    void emit_raw(cydui::events::Event* ev);
    
    void emit_raw(const str &event_type, void* data, std::function<void()> data_destructor);
    
    template<typename T>
    requires EventType<T>
    inline void emit(typename T::DataType data) {
      auto* data_ptr = new T({.data = data});
      emit_raw(T::type, data_ptr, [data_ptr](){ delete data_ptr; });
    }

    template<typename T> requires EventType<T>
    using Consumer = std::function<void(const ParsedEvent<T>&)>;
    
    typedef std::function<void(Event*)> Listener;
    struct listener_t;
    
    void remove_listener(const str &event_type, const listener_t &listener);
    
    struct listener_t {
    private:
      long ID;
      bool active = true;
    public:
      listener_t() {
        ID = 0;
        active = false;
      }
      
      explicit listener_t(str  type, Listener c): event_type(std::move(type)), func(new Listener {std::move(c)}) {
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
      Listener* func = nullptr;
      
      void remove() {
        cydui::events::remove_listener(event_type, *this);
        delete (u8*) ID;
        delete func;
        func = nullptr;
        ID = 0;
        active = false;
      }
      
      bool is_active() const {
        return active;
      }
      
      void operator()(Event* ev) const {
        if (nullptr != func) {
          func->operator()(ev);
        }
      }
    };
    
    listener_t* on_event_raw(const str &event_type, const Listener &l);
    
    template<typename T>
    requires EventType<T>
    inline listener_t* on_event(Consumer<T> c) {
      return cydui::events::on_event_raw(T::type, [&,c](Event* ev) {
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
#define listen(EVENT, ...) \
  cydui::events::on_event<EVENT>(cydui::events::Consumer<EVENT>( \
      [&](const cydui::events::ParsedEvent<EVENT>& it) __VA_ARGS__))


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
