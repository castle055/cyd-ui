// Copyright (c) 2024, Victor Castillo, All rights reserved.

#include "async/async_bus.h"

// ? Needed here because of the circular dependency with `event_queue`
void cydui::async::listener_t::remove() {
  if (nullptr != event_queue) event_queue->remove_listener(*this);
  delete func;
  func = nullptr;
  active = false;
}
