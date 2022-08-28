//
// Created by castle on 8/22/22.
//

#ifndef CYD_UI_WINDOW_TYPES_H
#define CYD_UI_WINDOW_TYPES_H

#include "../src/events/events.h"
#include "../src/graphics/graphics.h"

namespace cydui::layout {
  class Layout;
}
namespace cydui::window {
  class CWindow;

  class CWindowListener: public events::CEventListener {
    CWindow* win;

    void on_event(events::CEvent* ev) override;

  public:
    explicit CWindowListener(CWindow* win);
  };

  class CWindow {
  public:
    graphics::window_t* win_ref;
    CWindowListener* listener;
    layout::Layout* layout;
  };

}// namespace cydui::window
#endif//CYD_UI_WINDOW_TYPES_H
