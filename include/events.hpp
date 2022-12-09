//
// Created by castle on 8/21/22.
//

#ifndef CYD_UI_EVENTS_HPP
#define CYD_UI_EVENTS_HPP


#include <string>
#include <functional>

namespace cydui::events {
  template<class T>
  concept EventType = requires {
    T::type;
    typename T::DataType;
    T::data;
  };
  
  template<typename T> requires EventType<T>
  struct ParsedEvent {
    const std::string type;
    const typename T::DataType* data;
  };
  
  enum EventStatus {
    PENDING,
    PROCESSING,
    CONSUMED,
  };
  
  struct Event {
    std::string type;
    bool        consumed = false;
    EventStatus status   = PENDING;
    void* ev;
    
    template<typename T>
    requires EventType<T>
    ParsedEvent<T> parse() {
      std::string matchName = T::type;
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
  
  void emit_raw(const std::string &event_type, void* data);
  
  template<typename T>
  requires EventType<T>
  inline void emit(typename T::DataType data) {
    emit_raw(T::type, new T({.data = data}));
  }
  
  template<typename T> requires EventType<T>
  class Consumer: public std::function<void(const ParsedEvent<T> &)> {
  public:
    Consumer(std::function<void(const ParsedEvent<T> &)> c): std::function<void(const ParsedEvent<T> &)>(c) { }
  };

#define consumer [=](it)
  
  typedef std::function<void(Event*)> Listener;
  
  void on_event_raw(const std::string &event_type, const Listener &l);
  
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

#define listen(EVENT, block) cydui::events::on_event<EVENT>(cydui::events::Consumer<EVENT>([&](const cydui::events::ParsedEvent<EVENT>& it) block));

#define EVENT(NAME, DATA) \
struct NAME { \
constexpr static const char* type = #NAME; \
struct DataType DATA data; \
}; \

  
  void start();
  
}// namespace cydui::events

#endif//CYD_UI_EVENTS_HPP
