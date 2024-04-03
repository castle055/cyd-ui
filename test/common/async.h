//
// Created by castle on 4/3/24.
//

#ifndef CYD_UI_ASYNC_H
#define CYD_UI_ASYNC_H

#include "common.h"


namespace test::async {
    std::unique_ptr<cydui::async::async_bus_t> make_async_bus() {
      return std::make_unique<cydui::async::async_bus_t>();
    };
    std::unique_ptr<cydui::async::event_queue_t> make_event_queue() {
      return std::make_unique<cydui::async::event_queue_t>();
    };
    std::unique_ptr<cydui::async::coroutine_runtime_t> make_coroutine_runtime() {
      return std::make_unique<cydui::async::coroutine_runtime_t>();
    };
}


#endif //CYD_UI_ASYNC_H
