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
  
  struct Event {
    std::string type;
    bool consumed = false;
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
  
  void emit_raw(std::string event_type, void* data);
  
  template<typename T>
  requires EventType<T>
  inline void emit(typename T::DataType data) {
    emit_raw(T::type, new T({.data = data}));
  }
  
  template<typename T> requires EventType<T>
  class Consumer: public std::function<void(ParsedEvent<T>)> {
  public:
    Consumer(std::function<void(ParsedEvent<T>)> c): std::function<void(ParsedEvent<T>)>(c) { }
  };

#define consumer [=](it)->void
  
  void on_event(std::string event_type, std::function<void(Event)> c);
  
  template<typename T>
  requires EventType<T>
  inline void on_event(Consumer<T> c) {
    on_event(T::type, [c](Event ev) {
      auto parsed = ev.parse<T>();
      if (parsed.data) {
        c(parsed);
      }
    });
  }

#define listen(EVENT, block) cydui::events::onEvent<EVENT>(Consumer<EVENT>([](EVENT it)->void block));


#define EVENT(NAME, DATA) \
struct NAME { \
constexpr static const char* type = #NAME; \
struct DataType DATA data; \
}; \

  
  void start();
  
  //enum CEventType {
  //  EVENT_GRAPHICS,
  //  EVENT_LAYOUT,
  //  EVENT_CUSTOM,
  //};
  //enum CEventMode {
  //  EV_MODE_QUEUE,
  //  EV_MODE_STATE,
  //};
  //struct CEvent {
  //  const CEventType  type     = EVENT_CUSTOM;
  //  const CEventMode  mode     = EV_MODE_QUEUE;
  //  const std::string event_id = "";
  //  bool              consumed = false;
  //  void* raw_event = nullptr;
  //  void* data;
  //  unsigned long win = 0L;
  //};
  //
  //class CEventListener {
  //public:
  //  virtual void on_event(CEvent* ev) = 0;
  //};
  
  //void subscribe(CEventListener* listener);
  
  //void unsubscribe(CEventListener* listener);
  
  // Must be thread safe
  //void emit(CEvent* ev);
  
  //namespace layout {
  //  enum CLayoutEventType {
  //    LYT_EV_REDRAW,
  //    LYT_EV_KEYPRESS,
  //    LYT_EV_KEYRELEASE,
  //    LYT_EV_BUTTONPRESS,
  //    LYT_EV_BUTTONRELEASE,
  //    LYT_EV_MOUSEMOTION,
  //    LYT_EV_RESIZE,
  //    LYT_EV_UPDATE_PROP,
  //  };
  //  struct CRedrawEvent {
  //    const int x, y;
  //    const void* component = nullptr;
  //  };
  //  struct CKeyEvent {
  //    const unsigned int key;
  //  };
  //  struct CButtonEvent {
  //    const unsigned int button;
  //    const int          x, y;
  //  };
  //  struct CMotionEvent {
  //    int  x, y;
  //    bool enter = false;
  //    bool exit  = false;
  //  };
  //  struct CResizeEvent {
  //    const int w, h;
  //  };
  //  struct CUpdatePropEvent {
  //    const void* target_property = nullptr;
  //    const void* new_value       = nullptr;
  //  };
  //  union CLayoutData {
  //    CRedrawEvent     redraw_ev;
  //    CKeyEvent        key_ev;
  //    CButtonEvent     button_ev;
  //    CMotionEvent     motion_ev;
  //    CResizeEvent     resize_ev;
  //    CUpdatePropEvent update_prop_ev;
  //  };
  //  struct CLayoutEvent {
  //    const CLayoutEventType type;
  //    CLayoutData            data;
  //    void* win = nullptr;
  //    bool consumed = false;
  //  };
  //}// namespace layout
  //
  //namespace graphics {
  //  enum CGraphicEventType {
  //    GPH_EV_RESIZE,
  //  };
  //  struct CResizeEvent {
  //    const int w, h;
  //  };
  //  union CGraphicEventData {
  //    CResizeEvent resize_ev;
  //  };
  //  struct CGraphicsEvent {
  //    const CGraphicEventType type;
  //    CGraphicEventData       data;
  //    void* win = nullptr;
  //  };
  //}// namespace graphics
}// namespace cydui::events
//
//EVENT(SomeEvent, {
//  int someVal = 0;
//})
//
//void test() {
//
//  emit<SomeEvent>({
//    .someVal = 2
//  });
//}
//
//void someFun(Event2 ev) {
//
//  auto parsed = ev.parse<SomeEvent>();
//  if (parsed.data) {
//    parsed.data->someVal;
//  }
//
//}
//
//void otherFun() {
//  listen(SomeEvent, {
//
//  })
//}


#endif//CYD_UI_EVENTS_HPP
