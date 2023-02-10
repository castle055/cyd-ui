//
// Created by castle on 8/21/22.
//

#include "../../events.hpp"
#include "cyd-log/dist/include/logging.hpp"
#include "threading.hpp"
#include "../state/state.hpp"
#include <X11/Xlib.h>
#include <X11/keysym.h>

#include <utility>

cydui::threading::thread_t* x11_thread;

logging::logger x11_evlog = {.name = "X11::EV"};
logging::logger chev_log = {.name = "EV::CHANGE", .on = false};

using namespace std::chrono_literals;

Bool evpredicate() {
  return True;
}

cydui::events::change_ev::DataMonitor<RedrawEvent>
  redrawEventDataMonitor([](RedrawEvent::DataType o_data, RedrawEvent::DataType n_data) {
  // this event doesn't really hold data when emitted from x11::events so just consider it changed every time
  // it still reuses the same event object, so it won't overload the event bus
  return true;
});

cydui::events::change_ev::DataMonitor<ResizeEvent>
  resizeEventDataMonitor([](ResizeEvent::DataType o_data, ResizeEvent::DataType n_data) {
  return (o_data.w != n_data.w || o_data.h != n_data.h);
});

cydui::events::change_ev::DataMonitor<MotionEvent>
  motionEventDataMonitor([](MotionEvent::DataType o_data, MotionEvent::DataType n_data) {
  return true;
});

static std::unordered_map<unsigned int, Key> xkey_map = {
  {XK_a, Key::A},
  {XK_b, Key::B},
  {XK_c, Key::C},
  {XK_d, Key::D},
  {XK_e, Key::E},
  {XK_f, Key::F},
  {XK_g, Key::G},
  {XK_h, Key::H},
  {XK_i, Key::I},
  {XK_j, Key::J},
  {XK_k, Key::K},
  {XK_l, Key::L},
  {XK_m, Key::M},
  {XK_n, Key::N},
  {XK_o, Key::O},
  {XK_p, Key::P},
  {XK_q, Key::Q},
  {XK_r, Key::R},
  {XK_s, Key::S},
  {XK_t, Key::T},
  {XK_u, Key::U},
  {XK_v, Key::V},
  {XK_w, Key::W},
  {XK_x, Key::X},
  {XK_y, Key::Y},
  {XK_z, Key::Z},
  {XK_space, Key::SPACE},
  {XK_ISO_Enter, Key::ENTER},
  {XK_Escape, Key::ESC},
};

static void run() {
  XEvent ev;
  
  int queued = XEventsQueued(state::get_dpy(), QueuedAlready);
  for (int i = 0; i < queued; ++i) {
    XNextEvent(
      state::get_dpy(),
      &ev
    );
    //x11_evlog.debug("event = %d", ev.type);
    using namespace cydui::events;
    switch (ev.type) {
      case VisibilityNotify:
      case MapNotify:break;
      case Expose:
        redrawEventDataMonitor.update({
          .x = 0,
          .y = 0,
        });
        if (ev.xvisibility.type == Expose
          && ev.xexpose.count == 0
          /*&& ev.xexpose.width > 0
          && ev.xexpose.height > 0*/) {
          resizeEventDataMonitor.update({
            .win = (unsigned int) ev.xexpose.window,
            .w = ev.xexpose.width,
            .h = ev.xexpose.height,
          });
        }
        break;
      case KeyPress:x11_evlog.warn("KEY= %d", ev.xkey.keycode);
        if (xkey_map.contains(ev.xkey.keycode)) {
          x11_evlog.warn("====FOUND");
          emit<KeyEvent>({
            .win = (unsigned int) ev.xkey.window,
            .key = xkey_map[ev.xkey.keycode],
            .pressed = true,
          });
        }
        break;
      case KeyRelease:
        if (xkey_map.contains(ev.xkey.keycode)) {
          emit<KeyEvent>({
            .win = (unsigned int) ev.xkey.window,
            .key = xkey_map[ev.xkey.keycode],
            .released = true,
          });
        }
        break;
      case ButtonPress://x11_evlog.warn("BUTTON= %d", ev.xbutton.button);
        if (ev.xbutton.button == 4) {
          emit<ScrollEvent>({
            .win = (unsigned int) ev.xbutton.window,
            .d = 64,
            .x      = ev.xbutton.x,
            .y      = ev.xbutton.y,
          });
        } else if (ev.xbutton.button == 5) {
          emit<ScrollEvent>({
            .win = (unsigned int) ev.xbutton.window,
            .d = -64,
            .x      = ev.xbutton.x,
            .y      = ev.xbutton.y,
          });
        } else {
          emit<ButtonEvent>({
            .win = (unsigned int) ev.xbutton.window,
            .button = ev.xbutton.button,
            .x      = ev.xbutton.x,
            .y      = ev.xbutton.y,
            .pressed = true,
          });
        }
        break;
      case ButtonRelease:
        emit<ButtonEvent>({
          .win = (unsigned int) ev.xbutton.window,
          .button = ev.xbutton.button,
          .x      = ev.xbutton.x,
          .y      = ev.xbutton.y,
          .released = true,
        });
        break;
      case MotionNotify://x11_evlog.info("%d-%d", ev.xmotion.x, ev.xmotion.y);
        motionEventDataMonitor.update({
          .win = (unsigned int) ev.xmotion.window,
          .x = ev.xmotion.x,
          .y = ev.xmotion.y,
        });
        break;
      case ConfigureNotify://x11_evlog.info("%d-%d", ev.xconfigure.width, ev.xconfigure.height);
        resizeEventDataMonitor.update({
          .win = (unsigned int) ev.xconfigure.window,
          .w = ev.xconfigure.width,
          .h = ev.xconfigure.height,
        });
        break;
      case EnterNotify:break;
      case LeaveNotify:
        motionEventDataMonitor.update({
          .win = (unsigned int) ev.xcrossing.window,
          .x = -1,
          .y = -1,
        });
        break;
        break;
      case FocusIn:
      case FocusOut:
      case KeymapNotify:
      case GraphicsExpose:
      case NoExpose:
      case CreateNotify:
      case DestroyNotify:
      case UnmapNotify:
      case ResizeRequest:
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
