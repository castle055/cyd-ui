//
// Created by castle on 8/21/22.
//

#include "events.hpp"
#include "cydstd/logging.hpp"
#include "../state/state.hpp"
#include <X11/Xlib.h>
#include <X11/keysym.h>

cydui::threading::thread_t* x11_thread;

logging::logger x11_evlog = {.name = "X11::EV"};
logging::logger chev_log = {.name = "EV::CHANGE", .on = false};

using namespace std::chrono_literals;
using namespace x11;

Bool evpredicate() {
  return True;
}

cydui::events::change_ev::DataMonitor<RedrawEvent>
  redrawEventDataMonitor([](RedrawEvent::DataType &o_data, RedrawEvent::DataType &n_data) {
  // this event doesn't really hold data when emitted from x11::events so just consider it changed every time
  // it still reuses the same event object, so it won't overload the event bus
  return true;
});

cydui::events::change_ev::DataMonitor<ResizeEvent>
  resizeEventDataMonitor([](ResizeEvent::DataType &o_data, ResizeEvent::DataType &n_data) {
  return (o_data.w != n_data.w || o_data.h != n_data.h || o_data.win != n_data.win);
  //return true;
});

cydui::events::change_ev::DataMonitor<MotionEvent>
  motionEventDataMonitor([](MotionEvent::DataType &o_data, MotionEvent::DataType &n_data) {
  return true;
});

/*!
 * @brief This prevents the event thread from chocking on scroll events
 * @note It does impose a limit on the scroll speed to 64 units per frame
 * in either direction
 */
cydui::events::change_ev::DataMonitor<ScrollEvent>
  vScrollEventDataMonitor([](ScrollEvent::DataType &o_data, ScrollEvent::DataType &n_data) {
  n_data.dy += o_data.dy;
  return true;
}, [](ScrollEvent::DataType &data) {
  data.dy = 0;
});

/*!
 * @brief This prevents the event thread from chocking on scroll events
 * @note It does impose a limit on the scroll speed to 64 units per frame
 * in either direction
 */
cydui::events::change_ev::DataMonitor<ScrollEvent>
  hScrollEventDataMonitor([](ScrollEvent::DataType &o_data, ScrollEvent::DataType &n_data) {
  n_data.dx += o_data.dx;
  return true;
}, [](ScrollEvent::DataType &data) {
  data.dx = 0;
});

static std::unordered_map<KeySym, Key> xkey_map = {
  {XK_a,         Key::A},
  {XK_b,         Key::B},
  {XK_c,         Key::C},
  {XK_d,         Key::D},
  {XK_e,         Key::E},
  {XK_f,         Key::F},
  {XK_g,         Key::G},
  {XK_h,         Key::H},
  {XK_i,         Key::I},
  {XK_j,         Key::J},
  {XK_k,         Key::K},
  {XK_l,         Key::L},
  {XK_m,         Key::M},
  {XK_n,         Key::N},
  {XK_o,         Key::O},
  {XK_p,         Key::P},
  {XK_q,         Key::Q},
  {XK_r,         Key::R},
  {XK_s,         Key::S},
  {XK_t,         Key::T},
  {XK_u,         Key::U},
  {XK_v,         Key::V},
  {XK_w,         Key::W},
  {XK_x,         Key::X},
  {XK_y,         Key::Y},
  {XK_z,         Key::Z},
  {XK_space,     Key::SPACE},
  {XK_ISO_Enter, Key::ENTER},
  {XK_KP_Enter,  Key::ENTER},
  {XK_Return,    Key::ENTER},
  {XK_BackSpace, Key::BACKSPACE},
  {XK_Delete,    Key::DELETE},
  {XK_Escape,    Key::ESC},
  {XK_Left,      Key::LEFT},
  {XK_Right,     Key::RIGHT},
  {XK_Up,        Key::UP},
  {XK_Down,      Key::DOWN},
  {XK_Control_L, Key::LEFT_CTRL},
  {XK_Control_R, Key::RIGHT_CTRL},
  {XK_Shift_L,   Key::LEFT_SHIFT},
  {XK_Shift_R,   Key::RIGHT_CTRL},
  {XK_Super_L,   Key::LEFT_SUPER},
  {XK_Super_R,   Key::RIGHT_SUPER},
};

char input_buffer[10];
static XIM xim;
static XIC xic;
Status st;
KeySym ksym;

static void run() {
  XEvent ev;
  
  int queued = XEventsQueued(state::get_dpy(), QueuedAlready);
  for (int i = 0; i < queued; ++i) {
    XNextEvent(
      state::get_dpy(),
      &ev
    );
    //if (6 != ev.type
    //  && 2 != ev.type
    //  && 3 != ev.type
    //  )
    //x11_evlog.debug("event = %d", ev.type);
    using namespace cydui::events;
    switch (ev.type) {
      case MapNotify:
        break;
      case VisibilityNotify:
      case Expose:
        if (ev.xvisibility.type == VisibilityNotify) {
          redrawEventDataMonitor.update({
            .win = (unsigned int) ev.xvisibility.window,
          });
        } else if (ev.xexpose.type == Expose
          && ev.xexpose.count == 0
          /*&& ev.xexpose.width > 0
          && ev.xexpose.height > 0*/) {
          redrawEventDataMonitor.update({
            .win = (unsigned int) ev.xexpose.window,
          });
        }
        break;
      case KeyPress://x11_evlog.warn("KEY= %X", XLookupKeysym(&ev.xkey, 0));
        Xutf8LookupString(xic, &ev.xkey, input_buffer, 10, &ksym, &st);
        //x11_evlog.warn("BUF(%d)= %s", st, input_buffer);
        if ((st == XLookupKeySym || st == XLookupBoth)) {
          //x11_evlog.warn("====FOUND");
          // TODO
          // cydui::graphics::get_from_id(ev.xkey.window)->bus->emit<KeyEvent>({});
          emit<KeyEvent>({
            .win = (unsigned int) ev.xkey.window,
            .key = xkey_map.contains(ksym) ? xkey_map[ksym] : Key::UNKNOWN,
            .pressed = true,
            .text = st == XLookupBoth ? str(input_buffer) : "",
          });
        }
        break;
      case KeyRelease:
        if (xkey_map.contains(XLookupKeysym(&ev.xkey, 0))) {
          emit<KeyEvent>({
            .win = (unsigned int) ev.xkey.window,
            .key = xkey_map[XLookupKeysym(&ev.xkey, 0)],
            .released = true,
          });
        }
        break;
      case ButtonPress:
        //x11_evlog.warn("BUTTON= %d", ev.xbutton.button);
        if (ev.xbutton.button == 4) {
          //emit<ScrollEvent>({
          //  .win = (unsigned int) ev.xbutton.window,
          //  .dy = 64,
          //  .x = ev.xbutton.x,
          //  .y = ev.xbutton.y,
          //});
          vScrollEventDataMonitor.update({
            .win = (unsigned int) ev.xbutton.window,
            .dy = 64,
            .x = ev.xbutton.x,
            .y = ev.xbutton.y,
          });
        } else if (ev.xbutton.button == 5) {
          //emit<ScrollEvent>({
          //  .win = (unsigned int) ev.xbutton.window,
          //  .dy = -64,
          //  .x = ev.xbutton.x,
          //  .y = ev.xbutton.y,
          //});
          vScrollEventDataMonitor.update({
            .win = (unsigned int) ev.xbutton.window,
            .dy = -64,
            .x = ev.xbutton.x,
            .y = ev.xbutton.y,
          });
        } else if (ev.xbutton.button == 6) {
          hScrollEventDataMonitor.update({
            .win = (unsigned int) ev.xbutton.window,
            .dx = -64,
            .x = ev.xbutton.x,
            .y = ev.xbutton.y,
          });
        } else if (ev.xbutton.button == 7) {
          hScrollEventDataMonitor.update({
            .win = (unsigned int) ev.xbutton.window,
            .dx = 64,
            .x = ev.xbutton.x,
            .y = ev.xbutton.y,
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
        if (4 != ev.xbutton.button
          && 5 != ev.xbutton.button
          && 6 != ev.xbutton.button
          && 7 != ev.xbutton.button
          ) {
          emit<ButtonEvent>({
            .win = (unsigned int) ev.xbutton.window,
            .button = ev.xbutton.button,
            .x      = ev.xbutton.x,
            .y      = ev.xbutton.y,
            .released = true,
          });
        }
        break;
      case MotionNotify://x11_evlog.info("%d-%d", ev.xmotion.x, ev.xmotion.y);
        //x11_evlog.warn("%lX - MOTION", ev.xmotion.window);
        motionEventDataMonitor.update({
          .win = (unsigned int) ev.xmotion.window,
          .x = ev.xmotion.x,
          .y = ev.xmotion.y,
          .dragging = (ev.xmotion.state & Button1Mask) > 0,
        });
        break;
      case ConfigureNotify://x11_evlog.info("%d-%d", ev.xconfigure.width, ev.xconfigure.height);
        resizeEventDataMonitor.update({
          .win = (unsigned int) ev.xconfigure.window,
          .w = ev.xconfigure.width,
          .h = ev.xconfigure.height,
        });
        break;
      case EnterNotify:
        break;
      case LeaveNotify:
        //! I give no chance for this event not to be emitted
        emit<MotionEvent>({
          .win = (unsigned int) ev.xcrossing.window,
          .x = -1,
          .y = -1,
        });
        redrawEventDataMonitor.update({
          .win = (unsigned int) ev.xcrossing.window,
        });
        break;
      case FocusIn:
        //x11_evlog.error("%lX - FOCUS IN", ev.xfocus.window);
        break;
      case FocusOut:
        //x11_evlog.error("%lX - FOCUS OUT", ev.xfocus.window);
        break;
      case KeymapNotify:
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
      default:
        break;
    }
  }
  XFlush(state::get_dpy());
}

using namespace std::chrono_literals;

void x11_event_emitter_task(cydui::threading::thread_t* this_thread) {
  xim = XOpenIM(state::get_dpy(), NULL, NULL, NULL);
  xic = XCreateIC(xim,
    XNInputStyle, XIMPreeditNothing | XIMStatusNothing,
    NULL
  );
  while (this_thread->running) {
    run();
    std::this_thread::sleep_for(20ms);
  }
  XDestroyIC(xic);
  XCloseIM(xim);
}

void x11::events::start() {
  if (x11_thread && x11_thread->native_thread != nullptr)
    return;
  x11_evlog.debug("starting x11_thread");
  x11_thread = cydui::threading::new_thread(x11_event_emitter_task)
    ->set_name("X11_EV_THD");
}
