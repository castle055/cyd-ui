//
// Created by castle on 4/3/24.
//

#include "async.h"
#include <cassert>

std::unique_ptr<cydui::async::event_queue_t> ev_q = test::async::make_event_queue();

void setup() {
}

EVENT(TestEvent, {std::string test_data;})
TEST("Nominal Initialization") (
/// Just poke everything
  assert(ev_q->front_event_queue.empty());
  assert(ev_q->back_event_queue.empty());
  assert(ev_q->event_listeners.empty());
  assert(ev_q->event_mutex.try_lock());
  ev_q->event_mutex.unlock();
  assert(ev_q->listeners_mutex.try_lock());
  ev_q->listeners_mutex.unlock();
  
  return 0;
)
TEST("Nominal Event Emission") (
  assert(ev_q->front_event_queue.empty());
  ev_q->emit<TestEvent>({.test_data = "hello"});
  assert(ev_q->front_event_queue.size() == 1);
  assert(ev_q->front_event_queue.front()->parse<TestEvent>().type == "TestEvent");
  assert(ev_q->front_event_queue.front()->parse<TestEvent>().data->test_data == "hello");
  
  return 0;
)
TEST("Nominal Multiple Event Emission") (
  std::vector<std::string> test_data {
    "hello 1",
    "hello 2",
    "hello 3",
    "hello 4",
    "hello 5",
  };
  
  assert(ev_q->front_event_queue.empty());
  
  for (auto &item: test_data) {
    ev_q->emit<TestEvent>({.test_data = item});
  }
  assert(ev_q->front_event_queue.size() == test_data.size());
  
  assert(ev_q->front_event_queue.front()->parse<TestEvent>().type == "TestEvent");
  assert(ev_q->front_event_queue.front()->parse<TestEvent>().data->test_data == test_data[0]);
  
  return 0;
)
TEST("Nominal Queue Swapping") (
  assert(ev_q->front_event_queue.empty());
  assert(ev_q->back_event_queue.empty());
  ev_q->emit<TestEvent>({.test_data = "hello"});
  assert(ev_q->front_event_queue.size() == 1);
  assert(ev_q->back_event_queue.empty());
  
  ev_q->swap_event_queues();
  
  assert(ev_q->front_event_queue.empty());
  assert(ev_q->back_event_queue.size() == 1);
  
  return 0;
)
TEST("Nominal Listener Addition") (
  assert(ev_q->event_listeners.empty());
  
  auto* listener = ev_q->on_event<TestEvent>(cydui::async::Consumer<TestEvent>(
    [](const cydui::async::ParsedEvent<TestEvent> &ev) { }
  ));
  
  assert(ev_q->event_listeners.size() == 1);
  assert(ev_q->event_listeners.contains(TestEvent::type));
  assert(ev_q->event_listeners[TestEvent::type].size() == 1);
  assert(ev_q->event_listeners[TestEvent::type].front()->get_id() == listener->get_id());
  assert(ev_q->event_listeners[TestEvent::type].front()->event_type == str {TestEvent::type});
  assert(ev_q->event_listeners[TestEvent::type].front()->func == listener->func);
  assert(ev_q->event_listeners[TestEvent::type].front()->is_active());
  
  return 0;
)
TEST("Nominal Listener Removal") (
  assert(ev_q->event_listeners.empty());
  
  auto* listener = ev_q->on_event<TestEvent>(cydui::async::Consumer<TestEvent>(
    [](const cydui::async::ParsedEvent<TestEvent> &ev) { }
  ));
  
  assert(ev_q->event_listeners.size() == 1);
  assert(ev_q->event_listeners.contains(TestEvent::type));
  assert(ev_q->event_listeners[TestEvent::type].size() == 1);
  assert(ev_q->event_listeners[TestEvent::type].front()->get_id() == listener->get_id());
  assert(ev_q->event_listeners[TestEvent::type].front()->event_type == str {TestEvent::type});
  assert(ev_q->event_listeners[TestEvent::type].front()->func == listener->func);
  assert(ev_q->event_listeners[TestEvent::type].front()->is_active());
  
  listener->remove();
  delete listener;
  
  assert(ev_q->event_listeners.size() == 1);
  assert(ev_q->event_listeners.contains(TestEvent::type));
  assert(ev_q->event_listeners[TestEvent::type].empty());
  
  return 0;
)
TEST("Nominal Event Dispatch") (
  bool processed = false;
  
  // Add a listener that will check the event is dispatched correctly
  auto* listener = ev_q->on_event<TestEvent>(cydui::async::Consumer<TestEvent>(
    [&](const cydui::async::ParsedEvent<TestEvent> &ev) {
      if (ev.data->test_data == "hello.") {
        processed = true;
      }
    }
  ));
  
  // Emit test event
  ev_q->emit<TestEvent>({.test_data = "hello."});
  
  // Process all events in queue
  ev_q->events_process_batch();
  
  // Assert listener was in fact called with the correct data
  assert(processed);
  
  return 0;
)

TEST("Nominal Event Dispatch Ordering") (
  int processed = 0;
  
  // Add listeners that will check events are dispatched correctly AND in order
  ev_q->on_event<TestEvent>(cydui::async::Consumer<TestEvent>(
    [&](const cydui::async::ParsedEvent<TestEvent> &ev) {
      if (ev.data->test_data == "hello 1") {
        if (processed == 0) processed++;
      }
    }
  ));
  ev_q->on_event<TestEvent>(cydui::async::Consumer<TestEvent>(
    [&](const cydui::async::ParsedEvent<TestEvent> &ev) {
      if (ev.data->test_data == "hello 2") {
        if (processed == 1) processed++;
      }
    }
  ));
  ev_q->on_event<TestEvent>(cydui::async::Consumer<TestEvent>(
    [&](const cydui::async::ParsedEvent<TestEvent> &ev) {
      if (ev.data->test_data == "hello 3") {
        if (processed == 2) processed++;
      }
    }
  ));
  
  // Emit test events
  ev_q->emit<TestEvent>({.test_data = "hello 1"});
  ev_q->emit<TestEvent>({.test_data = "hello 2"});
  ev_q->emit<TestEvent>({.test_data = "hello 3"});
  
  // Process all events in queue
  ev_q->events_process_batch();
  
  // Assert listeners wer in fact called with the correct data AND in order
  assert(processed == 3);
  
  return 0;
)
