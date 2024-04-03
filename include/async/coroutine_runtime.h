// Copyright (c) 2024, Victor Castillo, All rights reserved.

#ifndef CYD_UI_COROUTINE_RUNTIME_H
#define CYD_UI_COROUTINE_RUNTIME_H

#include "coroutines.h"

#include <queue>

namespace cydui::async {
    class coroutine_runtime_t {
    private TEST_PUBLIC:
      std::queue<async_handle<>> coroutine_queue {};
      std::mutex queue_mtx {};
    
    protected TEST_PUBLIC: /// @name Bus Interface
      void coroutine_run() {
        std::optional<async_handle<>> current_handle;
        {
          std::scoped_lock lk {queue_mtx};
          if (coroutine_queue.empty()) {
            current_handle = std::nullopt;
          } else {
            current_handle = coroutine_queue.front();
            coroutine_queue.pop();
          }
        }
        
        current_handle.transform([](auto &&it) {
          it.resume();
          return it;
        });
      }
    public: /// @name Public Interface
      template<typename R>
      // ? Enqueue an already instantiated async handle
      async<R> &coroutine_enqueue(async<R> &handle) {
        std::scoped_lock lk {queue_mtx};
        coroutine_queue.push(handle.h_);
        return handle;
      }
      
      // ? Enqueue anything that is a coroutine, so anything that returns `async<>`
      template<
        typename C,
        typename ...Args,
        typename R = decltype(std::declval<C>()(std::declval<Args>()...))::return_type
      >
      requires requires(C &&c, Args &&...args) {
        {c(std::forward<Args>(args)...)} -> std::convertible_to<async<R>>;
      }
      async<R> coroutine_enqueue(C &&coroutine, Args &&... args) {
        auto a = coroutine(std::forward<Args>(args)...);
        coroutine_enqueue(a);
        return a;
      }
      
      // ? Anything that does not return `async<>` fails this static assertion
      template<typename C, typename ...Args, typename R = decltype(std::declval<C>()(std::declval<Args>()...))>
      inline void coroutine_enqueue(C &&coroutine, Args &&... args) {
        static_assert(false, "`coroutine` must return type async<>.");
      }
    public: /// @name Getter
      coroutine_runtime_t &get_coroutine_runtime() {
        return *this;
      }
    };
}

//using namespace cydui::async;
//struct lambda {
//  async<int> operator()() {
//    co_return 2;
//  }
//};
//async<int> testcoro(int a) {
//  co_return 2;
//}
//int testcoro2(int a) {
//  return 2;
//}
//
//void fasdfasdfsadf() {
//  using namespace cydui::async;
//
//  auto b = testcoro(2);
//  b();
//
//  coroutine_runtime_t cr {};
//
//  auto a = cr.coroutine_enqueue(testcoro, 12);
//  //cr.coroutine_enqueue(testcoro2, 12); // Fails and that is good
//  cr.coroutine_enqueue(lambda {});
//  cr.coroutine_enqueue([]() -> async<int> {
//    co_await testcoro(2);
//    co_return 1;
//  });/*.then([](int n) -> async<str> {
//    co_return "";
//  }).then([](str d) -> async<int> {
//    co_return 2;
//  });*/
//
//  []() -> async<int> {
//    co_return 12;
//  }();
//
//}
//
#endif //CYD_UI_COROUTINE_RUNTIME_H
