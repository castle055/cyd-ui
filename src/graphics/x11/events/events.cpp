//
// Created by castle on 8/21/22.
//

#include <X11/Xlib.h>
#include "../../events.h"
#include "../state/state.h"
#include "../../../events/events.h"
#include "../../../threading/threading.h"
#include "../../../logging/logging.h"

cydui::threading::thread_t* x11_thread;

logging::logger x11_evlog = { .name = "X11::EV" };

static void emit_lyt(XEvent ev, cydui::events::layout::CLayoutEventType type, cydui::events::layout::CLayoutData data) {
  cydui::events::emit(
      new cydui::events::CEvent {
          .type = cydui::events::EVENT_LAYOUT,
          .raw_event = new XEvent(ev),
          .data = new cydui::events::layout::CLayoutEvent { .type = type, .data = data }
      });
}
static void emit_gph(XEvent ev, cydui::events::graphics::CGraphicEventType type, cydui::events::graphics::CGraphicEventData data) {
  cydui::events::emit(
      new cydui::events::CEvent {
          .type = cydui::events::EVENT_GRAPHICS,
          .raw_event = new XEvent(ev),
          .data = new cydui::events::graphics::CGraphicsEvent { .type = type, .data = data }
      });
}


Bool evpredicate() {
  return True;
}

void run() {
  XEvent ev;
  
  while (XCheckIfEvent(state::get_dpy(), &ev, (int (*)(Display*, XEvent*, XPointer)) evpredicate, NULL)) {
//    x11_evlog.debug("event = %d", ev.type);
    switch (ev.type) {
      case Expose:
//        cydui::events::emit(new cydui::events::CEvent {
//            .type = cydui::events::EVENT_GRAPHICS
//        });
        emit_lyt(
            ev,
            cydui::events::layout::LYT_EV_REDRAW,
            cydui::events::layout::CLayoutData {
                .redraw_ev = cydui::events::layout::CRedrawEvent { .x = 0, .y = 0 }
            });
        break;
      case KeyPress:
        emit_lyt(
            ev,
            cydui::events::layout::LYT_EV_KEYPRESS,
            cydui::events::layout::CLayoutData {
                .key_ev = cydui::events::layout::CKeyEvent { .key = ev.xkey.keycode }
            });
        break;
      case KeyRelease:
        emit_lyt(
            ev,
            cydui::events::layout::LYT_EV_KEYRELEASE,
            cydui::events::layout::CLayoutData {
                .key_ev = cydui::events::layout::CKeyEvent { .key = ev.xkey.keycode }
            });
        break;
      case ButtonPress:
        emit_lyt(
            ev,
            cydui::events::layout::LYT_EV_BUTTONPRESS,
            cydui::events::layout::CLayoutData {
                .button_ev = cydui::events::layout::CButtonEvent { .button = ev.xbutton.button }
            });
        break;
      case ButtonRelease:
        emit_lyt(
            ev,
            cydui::events::layout::LYT_EV_BUTTONRELEASE,
            cydui::events::layout::CLayoutData {
                .button_ev = cydui::events::layout::CButtonEvent { .button = ev.xbutton.button, .x = ev.xbutton.x, .y = ev.xbutton.y }
            });
        break;
      case MotionNotify:
        emit_lyt(
            ev,
            cydui::events::layout::LYT_EV_MOUSEMOTION,
            cydui::events::layout::CLayoutData {
                .motion_ev = cydui::events::layout::CMotionEvent { .x = ev.xmotion.x, .y = ev.xmotion.y }
            });
        break;
      case ConfigureNotify:
        emit_lyt(
            ev,
            cydui::events::layout::LYT_EV_RESIZE,
            cydui::events::layout::CLayoutData {
                .resize_ev = cydui::events::layout::CResizeEvent {
                    .w = ev.xconfigure.width,
                    .h = ev.xconfigure.height
                }
            });
        emit_gph(
            ev,
            cydui::events::graphics::GPH_EV_RESIZE,
            cydui::events::graphics::CGraphicEventData {
                .resize_ev = cydui::events::graphics::CResizeEvent {
                    .w = ev.xconfigure.width,
                    .h = ev.xconfigure.height
                }
            });
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
      default:
        break;
    }
  }
}

void x11_event_emitter_task(cydui::threading::thread_t* this_thread) {
  while (this_thread->running) {
    run();
  }
}

void cydui::graphics::events::start() {
  if (x11_thread && x11_thread->native_thread != nullptr) return;
  x11_evlog.debug("starting x11_thread");
  x11_thread = threading::new_thread(&x11_event_emitter_task);
}
