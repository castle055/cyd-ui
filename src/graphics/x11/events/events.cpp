//
// Created by castle on 8/21/22.
//

#include "../../events.hpp"
#include "../../../../include/logging.hpp"
#include "../../../threading/threading.hpp"
#include "../state/state.hpp"
#include <X11/Xlib.h>

#include <utility>

cydui::threading::thread_t* x11_thread;

logging::logger x11_evlog = {.name = "X11::EV"};

using namespace std::chrono_literals;

Bool evpredicate() {
  return True;
}

cydui::events::change_ev::DataMonitor<ResizeEvent> resizeEventDataMonitor([](ResizeEvent::DataType o_data, ResizeEvent::DataType n_data){
    return (o_data.w != n_data.w || o_data.h != n_data.h);
});

void run() {
  XEvent ev;
  
  int      queued = XEventsQueued(state::get_dpy(), QueuedAlready);
  for (int i      = 0; i < queued; ++i) {
    XNextEvent(
      state::get_dpy(),
      &ev
    );
    //    x11_evlog.debug("event = %d", ev.type);
    using namespace cydui::events;
    switch (ev.type) {
      case Expose:
        emit<RedrawEvent>({
          .x = 0,
          .y = 0,
        });
        break;
      case KeyPress:
        emit<KeyEvent>({
          .key = ev.xkey.keycode,
          .pressed = true,
        });
        break;
      case KeyRelease:
        emit<KeyEvent>({
          .key = ev.xkey.keycode,
          .released = true,
        });
        break;
      case ButtonPress:
        emit<ButtonEvent>({
          .button = ev.xbutton.button,
          .x      = ev.xbutton.x,
          .y      = ev.xbutton.y,
          .pressed = true,
        });
        break;
      case ButtonRelease:
        emit<ButtonEvent>({
          .button = ev.xbutton.button,
          .x      = ev.xbutton.x,
          .y      = ev.xbutton.y,
          .released = true,
        });
        break;
      case MotionNotify://x11_evlog.info("%d-%d", ev.xmotion.x, ev.xmotion.y);
        emit<MotionEvent>({
          .x = ev.xmotion.x,
          .y = ev.xmotion.y,
        });
        break;
      case ConfigureNotify://x11_evlog.info("%d-%d", ev.xconfigure.width, ev.xconfigure.height);
          // TODO - IMPLEMENT STATE-BASED EVENTS
          resizeEventDataMonitor.update({
                                                .w = ev.xconfigure.width,
                                                .h = ev.xconfigure.height,
                                        });
//          emit<ResizeEvent>({
//                                    .w = ev.xconfigure.width,
//                                    .h = ev.xconfigure.height,
//                            });
            //emit_gph_state(
            //  std::string("gph_resize_").append(std::to_string(ev.xany.window)),
            //  ev,
        //  cydui::events::graphics::GPH_EV_RESIZE,
        //  cydui::events::graphics::CGraphicEventData {
        //    .resize_ev = cydui::events::graphics::CResizeEvent {
        //      .w = ev.xconfigure.width, .h = ev.xconfigure.height
        //    }
        //  }
        //);
        //emit_lyt_state(
        //  std::string("lyt_resize_").append(std::to_string(ev.xany.window)),
        //  ev,
        //  cydui::events::layout::LYT_EV_RESIZE,
        //  cydui::events::layout::CLayoutData {
        //    .resize_ev = cydui::events::layout::CResizeEvent {
        //      .w = ev.xconfigure.width, .h = ev.xconfigure.height
        //    }
        //  }
        //);
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
    run();
    std::this_thread::sleep_for(20ms);
  }
}

void cydui::graphics::events::start() {
  if (x11_thread && x11_thread->native_thread != nullptr)
    return;
  x11_evlog.debug("starting x11_thread");
  x11_thread = threading::new_thread(x11_event_emitter_task)
    ->set_name("X11_EV_THD");
}
