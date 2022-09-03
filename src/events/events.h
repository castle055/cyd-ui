//
// Created by castle on 8/21/22.
//

#ifndef CYD_UI_EVENTS_H
#define CYD_UI_EVENTS_H


namespace cydui::events {
  void start();
  
  enum CEventType {
    EVENT_GRAPHICS,
    EVENT_LAYOUT,
    EVENT_CUSTOM,
  };
  struct CEvent {
    const CEventType type;
    bool             consumed = false;
    void* raw_event = nullptr;
    void* data;
  };
  
  class CEventListener {
  public:
    virtual void on_event(CEvent* ev) = 0;
  };
  
  void subscribe(CEventListener* listener);
  
  void unsubscribe(CEventListener* listener);
  
  // Must be thread safe
  void emit(CEvent* ev);
  
  namespace layout {
    enum CLayoutEventType {
      LYT_EV_REDRAW,
      LYT_EV_KEYPRESS,
      LYT_EV_KEYRELEASE,
      LYT_EV_BUTTONPRESS,
      LYT_EV_BUTTONRELEASE,
      LYT_EV_MOUSEMOTION,
      LYT_EV_RESIZE,
      LYT_EV_UPDATE_PROP,
    };
    struct CRedrawEvent {
      const int x, y;
      const void* component = nullptr;
    };
    struct CKeyEvent {
      const unsigned int key;
    };
    struct CButtonEvent {
      const unsigned int button;
      const int          x, y;
    };
    struct CMotionEvent {
      int  x, y;
      bool enter = false;
      bool exit  = false;
    };
    struct CResizeEvent {
      const int w, h;
    };
    struct CUpdatePropEvent {
      const void* target_property = nullptr;
      const void* new_value       = nullptr;
    };
    union CLayoutData {
      CRedrawEvent     redraw_ev;
      CKeyEvent        key_ev;
      CButtonEvent     button_ev;
      CMotionEvent     motion_ev;
      CResizeEvent     resize_ev;
      CUpdatePropEvent update_prop_ev;
    };
    struct CLayoutEvent {
      const CLayoutEventType type;
      CLayoutData            data;
      void* win = nullptr;
      bool consumed = false;
    };
  }// namespace layout
  
  namespace graphics {
    enum CGraphicEventType {
      GPH_EV_RESIZE,
    };
    struct CResizeEvent {
      const int w, h;
    };
    union CGraphicEventData {
      CResizeEvent resize_ev;
    };
    struct CGraphicsEvent {
      const CGraphicEventType type;
      CGraphicEventData       data;
      void* win = nullptr;
    };
  }// namespace graphics
}// namespace cydui::events


#endif//CYD_UI_EVENTS_H
