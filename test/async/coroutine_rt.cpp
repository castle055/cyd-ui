// Copyright (c) 2024, Victor Castillo, All rights reserved.

#include "async.h"
#include <cassert>

std::unique_ptr<cydui::async::coroutine_runtime_t> coro_rt = test::async::make_coroutine_runtime();

void setup() {
}

async<int> test_coroutine(int n = 0) {
  co_return n * 2;
}

TEST("Nominal Initialization") (
/// Just poke everything
  assert(coro_rt->coroutine_queue.empty());
  assert(coro_rt->queue_mtx.try_lock());
  coro_rt->queue_mtx.unlock();
  
  return 0;
)
TEST("Nominal Function Coroutine Queuing") (
  coro_rt->coroutine_enqueue(test_coroutine, 10);
  assert(coro_rt->coroutine_queue.size() == 1);
  
  return 0;
)
TEST("Nominal Lambda Coroutine Queuing") (
  coro_rt->coroutine_enqueue(
    [](int n) -> async<int> {
      co_return n * 2;
    }, 10
  );
  assert(coro_rt->coroutine_queue.size() == 1);
  
  return 0;
)
TEST("Nominal Pre-Existing Coroutine Queuing") (
  async<int> a = test_coroutine();
  async<int> b = coro_rt->coroutine_enqueue(a);
  assert(coro_rt->coroutine_queue.size() == 1);
  assert(a.h_.address() == b.h_.address());
  
  return 0;
)
TEST("Nominal Coroutine Handling") (
  async<int> a = coro_rt->coroutine_enqueue(test_coroutine, 10);
  assert(coro_rt->coroutine_queue.size() == 1);
  assert(!a.h_.done());
  
  coro_rt->coroutine_run();
  
  assert(a.h_.done());
  assert(a.h_.promise().future_.get() == 20);
  
  return 0;
)

// [TODO] Tests for co_await
