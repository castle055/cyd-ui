//
// Created by castle on 8/21/22.
//

#include "../../include/layout.h"
#include "../logging/logging.h"

logging::logger log = { .name = "LAYOUT", .on = false };

cydui::layout::Layout::Layout(cydui::components::Component *root)
  : root(root) {
}

void cydui::layout::Layout::bind_window(cydui::window::CWindow *win) {
  this->win = win;
}

void cydui::layout::Layout::on_event(cydui::events::layout::CLayoutEvent *ev) {
  //  log.debug("Event %d", ev->type);
  ev->win = (void*)win;
  switch (ev->type) {
    case events::layout::LYT_EV_REDRAW:
      if (ev->data.redraw_ev.component) {
        ((components::Component*)ev->data.redraw_ev.component)->on_event(ev);
      } else {
        root->on_event(ev);
      }
      break;
    case events::layout::LYT_EV_KEYPRESS:
      break;
    case events::layout::LYT_EV_KEYRELEASE:
      break;
    case events::layout::LYT_EV_BUTTONPRESS:
      break;
    case events::layout::LYT_EV_BUTTONRELEASE:
      break;
    case events::layout::LYT_EV_MOUSEMOTION:
//      log.debug("MOTION x=%d, y=%d", ev->data.motion_ev.x, ev->data.motion_ev.y);
      break;
    case events::layout::LYT_EV_RESIZE:
      log.debug("RESIZE w=%d, h=%d", ev->data.resize_ev.w, ev->data.resize_ev.h);
      root->on_event(ev);
      break;
    default:
      break;
  }
}
