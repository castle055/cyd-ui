/*! \file  events.cppm
 *! \brief 
 *!
 */

module;
#include <X11/Xlib.h>
#include <X11/keysym.h>

export module cydui.graphics.native:events;

import std;
import fabric.logging;
import cydui.events;

import :window;

cyd::ui::threading::thread_t* x11_thread;


using namespace std::chrono_literals;

Bool evpredicate() {
  return True;
}

//cyd::ui::async::change_ev::DataMonitor<RedrawEvent>
//  redrawEventDataMonitor([](RedrawEvent::DataType &o_data, RedrawEvent::DataType &n_data) {
//  // this event doesn't really hold data when emitted from x11::events so just consider it changed every time
//  // it still reuses the same event object, so it won't overload the event bus
//  return true;
//});
//
//cyd::ui::async::change_ev::DataMonitor<ResizeEvent>
//  resizeEventDataMonitor([](ResizeEvent::DataType &o_data, ResizeEvent::DataType &n_data) {
//  return (o_data.w != n_data.w || o_data.h != n_data.h || o_data.win != n_data.win);
//  //return true;
//});
//
//cyd::ui::async::change_ev::DataMonitor<MotionEvent>
//  motionEventDataMonitor([](MotionEvent::DataType &o_data, MotionEvent::DataType &n_data) {
//  return true;
//});
//
///*!
// * @brief This prevents the event thread from chocking on scroll events
// * @note It does impose a limit on the scroll speed to 64 units per frame
// * in either direction
// */
//cyd::ui::async::change_ev::DataMonitor<ScrollEvent>
//  vScrollEventDataMonitor([](ScrollEvent::DataType &o_data, ScrollEvent::DataType &n_data) {
//  n_data.dy += o_data.dy;
//  return true;
//}, [](ScrollEvent::DataType &data) {
//  data.dy = 0;
//});
//
///*!
// * @brief This prevents the event thread from chocking on scroll events
// * @note It does impose a limit on the scroll speed to 64 units per frame
// * in either direction
// */
//cyd::ui::async::change_ev::DataMonitor<ScrollEvent>
//  hScrollEventDataMonitor([](ScrollEvent::DataType &o_data, ScrollEvent::DataType &n_data) {
//  n_data.dx += o_data.dx;
//  return true;
//}, [](ScrollEvent::DataType &data) {
//  data.dx = 0;
//});

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
  {XK_Page_Up,   Key::PAGE_UP},
  {XK_Page_Down, Key::PAGE_DOWN},
  {XK_Home,      Key::HOME},
  {XK_End,       Key::END},
  {XK_F1,        Key::F1},
  {XK_F2,        Key::F2},
  {XK_F3,        Key::F3},
  {XK_F4,        Key::F4},
  {XK_F5,        Key::F5},
  {XK_F6,        Key::F6},
  {XK_F7,        Key::F7},
  {XK_F8,        Key::F8},
  {XK_F9,        Key::F9},
  {XK_F10,       Key::F10},
  {XK_F11,       Key::F11},
  {XK_F12,       Key::F12},
  {XK_F13,       Key::F13},
  {XK_F14,       Key::F14},
  {XK_F15,       Key::F15},
  {XK_F16,       Key::F16},
  {XK_F17,       Key::F17},
  {XK_F18,       Key::F18},
  {XK_F19,       Key::F19},
  {XK_F20,       Key::F20},
  {XK_F21,       Key::F21},
  {XK_F22,       Key::F22},
  {XK_F23,       Key::F23},
  {XK_F24,       Key::F24},
  {XK_F25,       Key::F25},
  {XK_F26,       Key::F26},
  {XK_F27,       Key::F27},
  {XK_F28,       Key::F28},
  {XK_F29,       Key::F29},
  {XK_F30,       Key::F30},
  {XK_F31,       Key::F31},
  {XK_F32,       Key::F32},
  {XK_F33,       Key::F33},
  {XK_F34,       Key::F34},
  {XK_F35,       Key::F35},
};

Status st;
KeySym ksym;

template<fabric::async::EventType EV>
static inline bool emit_to_window(unsigned long win, EV &&ev) {
  auto w = native::get_from_id(win).transform([&](cyd::ui::graphics::window_t* w) {
    w->bus->emit<EV>(ev);
    return w;
  });
  return w.has_value();
}

static void run() {
  XEvent ev;

  int queued = XEventsQueued(x11::state::get_dpy(), QueuedAlready);
  for (int i = 0; i < queued; ++i) {
    XNextEvent(
      x11::state::get_dpy(),
      &ev
    );
    if (XFilterEvent(&ev, None)) {
      continue;
    }
    //if (6 != ev.type
    //  && 2 != ev.type
    //  && 3 != ev.type
    //  )
    //x11_evlog.debug("event = %d", ev.type);
    switch (ev.type) {
      case MapNotify:
        break;
      case VisibilityNotify:
      case Expose:
        if (ev.xvisibility.type == VisibilityNotify) {
          emit_to_window<RedrawEvent>(ev.xvisibility.window, {}); // ! Should this be throttled?
        } else if (ev.xexpose.type == Expose
          && ev.xexpose.count == 0
          /*&& ev.xexpose.width > 0
          && ev.xexpose.height > 0*/) {
          native::get_from_id(ev.xexpose.window).transform([&](cyd::ui::graphics::window_t* w) {
            if (w->old_width != ev.xexpose.width || w->old_height != ev.xexpose.height) {
              w->old_width  = ev.xexpose.width;
              w->old_height = ev.xexpose.height;
              emit_to_window<ResizeEvent>(
                ev.xconfigure.window,
                {
                  .w = ev.xexpose.width,
                  .h = ev.xexpose.height,
                }
              ); // ! Should this be throttled?
            }
            return w;
          });
          emit_to_window<RedrawEvent>(ev.xexpose.window, {}); // ! Should this be throttled?
        }
        break;
      case KeyPress://x11_evlog.warn("KEY= %X", XLookupKeysym(&ev.xkey, 0));
        native::get_from_id(ev.xkey.window).transform([&](cyd::ui::graphics::window_t* w) {
            XmbLookupString(
              w->input_method.xic,
              &ev.xkey,
              w->input_method.input_buffer,
              sizeof w->input_method.input_buffer,
              &ksym,
              &st
            );
            if ((st == XLookupKeySym || st == XLookupBoth)) {
              emit_to_window<KeyEvent>(
                ev.xkey.window,
                {
                  .key     = xkey_map.contains(ksym) ? xkey_map[ksym] : Key::UNKNOWN,
                  .pressed = true,
                  .text    = st == XLookupBoth ? std::string(w->input_method.input_buffer) : "",
                }
              );
            }
            return w;
        });
        //x11_evlog.warn("BUF(%d)= %s", st, input_buffer);
        break;
      case KeyRelease:
        if (xkey_map.contains(XLookupKeysym(&ev.xkey, 0))) {
            emit_to_window<KeyEvent>(
              ev.xkey.window,
              {
                .key      = xkey_map[XLookupKeysym(&ev.xkey, 0)],
                .released = true,
              }
            );
        }
        break;
      case ButtonPress:
        //x11_evlog.warn("BUTTON= %d", ev.xbutton.button);
        if (ev.xbutton.button == 4) {
            // emit<ScrollEvent>({
            //   .win = (unsigned int) ev.xbutton.window,
            //   .dy = 64,
            //   .x = ev.xbutton.x,
            //   .y = ev.xbutton.y,
            // });
            emit_to_window<ScrollEvent>(
              ev.xbutton.window,
              {
                .dy = 64,
                .x  = ev.xbutton.x,
                .y  = ev.xbutton.y,
              }
            ); // ! Should this be throttled?
        } else if (ev.xbutton.button == 5) {
            // emit<ScrollEvent>({
            //   .win = (unsigned int) ev.xbutton.window,
            //   .dy = -64,
            //   .x = ev.xbutton.x,
            //   .y = ev.xbutton.y,
            // });
            emit_to_window<ScrollEvent>(
              ev.xbutton.window,
              {
                .dy = -64,
                .x  = ev.xbutton.x,
                .y  = ev.xbutton.y,
              }
            ); // ! Should this be throttled?
        } else if (ev.xbutton.button == 6) {
            emit_to_window<ScrollEvent>(
              ev.xbutton.window,
              {
                .dx = -64,
                .x  = ev.xbutton.x,
                .y  = ev.xbutton.y,
              }
            ); // ! Should this be throttled?
        } else if (ev.xbutton.button == 7) {
            emit_to_window<ScrollEvent>(
              ev.xbutton.window,
              {
                .dx = 64,
                .x  = ev.xbutton.x,
                .y  = ev.xbutton.y,
              }
            ); // ! Should this be throttled?
        } else {
            emit_to_window<ButtonEvent>(
              ev.xbutton.window,
              {
                .button  = ev.xbutton.button,
                .x       = ev.xbutton.x,
                .y       = ev.xbutton.y,
                .pressed = true,
              }
            );
        }
        break;
      case ButtonRelease:
        if (4 != ev.xbutton.button
          && 5 != ev.xbutton.button
          && 6 != ev.xbutton.button
          && 7 != ev.xbutton.button
          ) {
            emit_to_window<ButtonEvent>(
              ev.xbutton.window,
              {
                .button   = ev.xbutton.button,
                .x        = ev.xbutton.x,
                .y        = ev.xbutton.y,
                .released = true,
              }
            );
        }
        break;
      case MotionNotify://x11_evlog.info("%d-%d", ev.xmotion.x, ev.xmotion.y);
        //x11_evlog.warn("%lX - MOTION", ev.xmotion.window);
        emit_to_window<MotionEvent>(ev.xmotion.window, {
          .x = ev.xmotion.x,
          .y = ev.xmotion.y,
          .dragging = (ev.xmotion.state & Button1Mask) > 0,
        }); // ! Should this be throttled?
        break;
      case ConfigureNotify: // x11_evlog.info("%d-%d", ev.xconfigure.width, ev.xconfigure.height);
        native::get_from_id(ev.xconfigure.window).transform([&](cyd::ui::graphics::window_t* w) {
            if (w->old_width != ev.xconfigure.width || w->old_height != ev.xconfigure.height) {
              w->old_width  = ev.xconfigure.width;
              w->old_height = ev.xconfigure.height;
              emit_to_window<ResizeEvent>(
                ev.xconfigure.window,
                {
                  .w = ev.xconfigure.width,
                  .h = ev.xconfigure.height,
                }
              ); // ! Should this be throttled?
            }
            return w;
        });
        emit_to_window<RedrawEvent>(ev.xvisibility.window, {}); // ! Should this be throttled?
        break;
      case EnterNotify:
        break;
      case LeaveNotify:
        //! I give no chance for this event not to be emitted
        emit_to_window<MotionEvent>(ev.xcrossing.window, {
          .x = -1,
          .y = -1,
        });
        emit_to_window<RedrawEvent>(ev.xcrossing.window, {}); // ! Should this be throttled?
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
  XFlush(x11::state::get_dpy());
}

using namespace std::chrono_literals;

void x11_event_emitter_task(cyd::ui::threading::thread_t* this_thread) {
  while (this_thread->running) {
    run();
    // std::this_thread::sleep_for(10ms);
  }
}

export
namespace x11::events {
  void start() {
    if (x11_thread && x11_thread->native_thread != nullptr)
      return;
    LOG::print {DEBUG}("starting x11_thread");
    x11_thread = cyd::ui::threading::new_thread(x11_event_emitter_task)
      ->set_name("X11_EV_THD");
  }
}

