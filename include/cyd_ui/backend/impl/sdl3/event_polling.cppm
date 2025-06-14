/*! \file  event_polling.cppm
 *! \brief 
 *!
 */

export module cydui.backends.sdl3:event_polling;
import :frame_map;
import :window_events;

import std;
export import reflect;

import cydui.application;

namespace cydui::backends {
  export class event_polling_system {
    frame_map::sptr registered_frame_map_{std::make_shared<frame_map>()};
    bool            running_;
    fabric::task<>  task_;

  public:
    event_polling_system(frame_map::sptr registered_frame_map)
        : registered_frame_map_(registered_frame_map),
          running_(true),
          task_(Application::schedule([&] -> fabric::task<> {
            co_await task_fun();
            co_return;
          })) {}

    ~event_polling_system() {
      running_ = false;
      task_.wait();
    }

  private:
    fabric::task<> task_fun() {
      using namespace std::chrono_literals;

      while (running_) { // will be stopped at the co_await when the executor is destroyed
        {
          poll_events(registered_frame_map_);
        }
        co_await 16ms;
      }
      co_return;
    }
  };

}

