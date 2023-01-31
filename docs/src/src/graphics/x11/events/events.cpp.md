//
// Created by castle on 8/21/22.
//

#include "../../events.hpp"
#include "../../../events/events.hpp"
#include "../../../logging/logging.hpp"
#include "../../../threading/threading.hpp"
#include "../state/state.hpp"
#include <X11/Xlib.h>

cydui::threading::thread_t* x11_thread;

logging::logger x11_evlog = {.name = "X11::EV"};

static void emit_lyt(
    XEvent data,
    cydui::events::layout::CLayoutEventType type,
    cydui::events::layout::CLayoutData data
) {
  cydui::events::emit(
      new cydui::events::CEvent {
          .type      = cydui::events::EVENT_LAYOUT,
          .raw_event = new XEvent(data),
          .data      = new cydui::events::layout::CLayoutEvent {
              .type = type, .data = data
          }
      }
  );
}

static void emit_gph(
    XEvent data,
    cydui::events::graphics::CGraphicEventType type,
    cydui::events::graphics::CGraphicEventData data
) {
  cydui::events::emit(
      new cydui::events::CEvent {
          .type      = cydui::events::EVENT_GRAPHICS,
          .raw_event = new XEvent(data),
          .data      = new cydui::events::graphics::CGraphicsEvent {
              .type = type, .data = data
          }
      }
  );
}

using namespace std::chrono_literals;
static auto conf_notif_timeout   = 100ms;
static auto conf_notif_last_time = std::chrono::system_clock::now().time_since_epoch();

Bool evpredicate() {
  return True;
}

void start_thd() {
  XEvent data;
  
  int      queued = XEventsQueued(state::get_dpy(), QueuedAlready);
  for (int i      = 0; i < queued; ++i) {
    XNextEvent(
        state::get_dpy(),
        &data
    );
    //    x11_evlog.debug("event = %d", data.type);
    switch (data.type) {
      case Expose:
        //        cydui::events::emit(new cydui::events::CEvent {
        //            .type = cydui::events::EVENT_GRAPHICS
        //        });
        emit_lyt(
            data,
            cydui::events::layout::LYT_EV_REDRAW,
            cydui::events::layout::CLayoutData {
                .redraw_ev =
                cydui::events::layout::CRedrawEvent {.x = 0, .y = 0}
            }
        );
        break;
      case KeyPress:
        emit_lyt(
            data,
            cydui::events::layout::LYT_EV_KEYPRESS,
            cydui::events::layout::CLayoutData {
                .key_ev =
                cydui::events::layout::CKeyEvent {.key = data.xkey.keycode}}
        );
        break;
      case KeyRelease:
        emit_lyt(
            data,
            cydui::events::layout::LYT_EV_KEYRELEASE,
            cydui::events::layout::CLayoutData {
                .key_ev =
                cydui::events::layout::CKeyEvent {.key = data.xkey.keycode}}
        );
        break;
      case ButtonPress:
        emit_lyt(
            data,
            cydui::events::layout::LYT_EV_BUTTONPRESS,
            cydui::events::layout::CLayoutData {
                .button_ev = cydui::events::layout::CButtonEvent {
                    .button = data.xbutton.button,
                    .x      = data.xbutton.x,
                    .y      = data.xbutton.y
                }}
        );
        break;
      case ButtonRelease:
        emit_lyt(
            data,
            cydui::events::layout::LYT_EV_BUTTONRELEASE,
            cydui::events::layout::CLayoutData {
                .button_ev = cydui::events::layout::CButtonEvent {
                    .button = data.xbutton.button,
                    .x      = data.xbutton.x,
                    .y      = data.xbutton.y
                }
            }
        );
        break;
      case MotionNotify://x11_evlog.info("%d-%d", data.xmotion.x, data.xmotion.y);
        emit_lyt(
            data,
            cydui::events::layout::LYT_EV_MOUSEMOTION,
            cydui::events::layout::CLayoutData {
                .motion_ev = cydui::events::layout::CMotionEvent {
                    .x = data.xmotion.x, .y = data.xmotion.y
                }
            }
        );
        break;
      case ConfigureNotify:
        emit_gph(
            data,
            cydui::events::graphics::GPH_EV_RESIZE,
            cydui::events::graphics::CGraphicEventData {
                .resize_ev = cydui::events::graphics::CResizeEvent {
                    .w = data.xconfigure.width, .h = data.xconfigure.height
                }
            }
        );
        emit_lyt(
            data,
            cydui::events::layout::LYT_EV_RESIZE,
            cydui::events::layout::CLayoutData {
                .resize_ev = cydui::events::layout::CResizeEvent {
                    .w = data.xconfigure.width, .h = data.xconfigure.height
                }
            }
        );
        break;
      case EnterNotify:
      case LeaveNotify:
      case FocusIn:
      case FocusOut:
      case KeymapNotify:
      case GraphicsExpose:
      case NoExpose:
      case VisibilityNotify:
      case CreateNotify:
      case DestroyNotify:
      case UnmapNotify:
      case ResizeRequest:
      case MapNotify:
      case MapRequest:
      case ReparentNotify:
      case ConfigureRequest:
      case GravityNotify:
      case CirculateNotify:
      case CirculateRequest:
      case PropertyNotify:
      case SelectionClear:
      case SelectionRequest:
      case SelectionNotify:
      case ColormapNotify:
      case ClientMessage:
      case MappingNotify:
      case GenericEvent:
      default: break;
    }
  }
  XFlush(state::get_dpy());
}

using namespace std::chrono_literals;

void x11_event_emitter_task(cydui::threading::thread_t* this_thread) {
  while (this_thread->running) {
    start_thd();
    std::this_thread::sleep_for(10ms);
  }
}

void cydui::graphics::events::start_thd() {
  if (x11_thread && x11_thread->native_thread != nullptr)
    return;
  x11_evlog.debug("starting x11_thread");
  x11_thread = threading::new_thread(&x11_event_emitter_task);
}
