//
// Created by castle on 8/21/22.
//

#ifndef CYD_UI_EVENTS_HPP
#define CYD_UI_EVENTS_HPP


#include <functional>
#include <mutex>
#include <string>

#include "cydstd/cydstd.h"

namespace cydui::events {
  template<class T>
  concept EventType = requires {
                        { T::type } -> std::convertible_to<str>;
                        typename T::DataType;
                        {
                          T::data
                        } -> std::convertible_to<typename T::DataType>;
                      };

  template<typename T>
    requires EventType<T>
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
            .data = (typename T::DataType*)ev,
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

  void emit_raw(const str& event_type, void* data);

  template<typename T>
    requires EventType<T>
  inline void emit(typename T::DataType data) {
    emit_raw(T::type, new T({.data = data}));
  }

  template<typename T>
    requires EventType<T>
  class Consumer: public std::function<void(const ParsedEvent<T>&)> {
  public:
#pragma clang diagnostic push
#pragma ide diagnostic ignored "google-explicit-constructor"
    Consumer(std::function<void(const ParsedEvent<T>&)> c)
        : std::function<void(const ParsedEvent<T>&)>(c) {
    }
#pragma clang diagnostic pop
  };


  typedef std::function<void(Event*)> Listener;

  void on_event_raw(const str& event_type, const Listener& l);

  template<typename T>
    requires EventType<T>
  inline void on_event(Consumer<T> c) {
    cydui::events::on_event_raw(T::type, [c](Event* ev) {
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

#define EVENT(NAME, DATA)                                                      \
  struct NAME {                                                                \
    constexpr static const char* type = #NAME;                                 \
    struct DataType DATA data;                                                 \
  };


#endif//CYD_UI_EVENTS_HPP
