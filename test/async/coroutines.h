//
// Created by castle on 12/11/23.
//

#ifndef CYD_UI_COROUTINES_H
#define CYD_UI_COROUTINES_H

#include <coroutine>
#include <future>
#include "../../include/cydstd/logging.hpp"

static logging::logger L {.name = "CORO"};

//template<typename PromiseType>
//struct get_promise {
//  PromiseType* p_;
//  bool await_ready() const noexcept {return false;}
//  bool await_suspend(std::coroutine_handle<PromiseType> h) {
//    p_ = &h.promise();
//    return false;
//  }
//  PromiseType* await_resume() {return p_;}
//};

template<typename Ret>
struct async;
class executor;

static std::unordered_map<std::thread::id, executor*> executors;

struct executor {
  executor() {
    exec_thd = std::thread {executor::executor_task, this};
    executors[exec_thd.get_id()] = this;
  }
  virtual ~executor() {
    is_running.clear();
    exec_thd.join();
    executors.erase(exec_thd.get_id());
  };
  
  static std::optional<executor*> from_this_thread() {
    auto id = std::this_thread::get_id();
    if (executors.contains(id)) {
      return executors.at(id);
    } else {
      return std::nullopt;
    }
  }
  
  virtual void queue(std::coroutine_handle<> task) = 0;

protected:
  virtual void loop() = 0;

private:
  std::thread exec_thd;
  std::atomic_flag is_running {true};
  
  static void executor_task(executor* e) {
    while (e->is_running.test()) {
      e->loop();
    }
  }
};

struct continuation {
  std::optional<std::coroutine_handle<>> cont;
  constexpr bool await_ready() const noexcept {return false;}
  
  std::coroutine_handle<> await_suspend(std::coroutine_handle<> h) noexcept {
    if (cont.has_value()) {
      return cont.value();
    } else {
      return std::noop_coroutine();//h;
    }
  }
  void await_resume() const noexcept { }
};

template<typename Ret>
struct async {
  struct promise_type {
    std::promise<Ret> value_promise_ {};
    std::shared_future<Ret> future_ = value_promise_.get_future().share();
    std::optional<std::coroutine_handle<>> cont = std::nullopt;
    
    async get_return_object() {
      return async {
        .h_ = std::coroutine_handle<promise_type>::from_promise(*this),
        .future_ = future_,
      };
    }
    std::suspend_always initial_suspend() {return {};}
    continuation final_suspend() noexcept {return {cont};}
    void unhandled_exception() { }
    
    std::suspend_always yield_value(bool a) {
      //value_promise_.set_value(value);
      return {};
    }
    
    void return_value(Ret value) {
      value_promise_.set_value(value);
    }
  };
  
  std::coroutine_handle<promise_type> h_;
  operator std::coroutine_handle<promise_type>() const {return h_;}
  
  std::shared_future<Ret> future_;
  
  bool await_ready() const noexcept {return false;}
  
  std::coroutine_handle<promise_type> await_suspend(std::coroutine_handle<> h) {
    //auto e = executor::from_this_thread();
    //if (e.has_value()) {
    //  e.value()->queue(h);
    //}
    h_.promise().cont = h;
    return h_;
  }
  Ret await_resume() {
    //while (!h_.done()) {
    //  h_.resume();
    //}
    return future_.get();
  }
};

class executor_t: executor {
  std::deque<std::coroutine_handle<>> tasks {};
  std::mutex tasks_mtx {};
  
  void loop() override {
    std::coroutine_handle<> current_handle;
    {
      std::lock_guard lk {tasks_mtx};
      if (!tasks.empty()) {
        current_handle = tasks.front();
        tasks.pop_front();
      }
    }
    
    if (!current_handle) {
      return;
    }
    
    current_handle.resume();
    
    if (!current_handle.done()) {
      //{
      //  std::lock_guard lk {tasks_mtx};
      //  tasks.push_back(current_handle);
      //}
    } else {
      current_handle.destroy();
    }
  }

public:
  
  void queue(std::coroutine_handle<> handle) override {
    std::lock_guard lk {tasks_mtx};
    tasks.push_back(handle);
  }
  
  template<typename R>
  async<R> launch(async<R> task) {
    queue(task.h_);
    return task;
  }
  
};


struct explicit_test {
  explicit(false) explicit_test() {
  
  }
};

async<long> get_file_size1(std::string path) {
  L.info("Start 3 %s", path.c_str());
  
  L.info("End 3 %s", path.c_str());
  co_return 4;
}
async<long> get_file_size(std::string path) {
  L.info("Start 2 %s", path.c_str());
  
  auto size = co_await get_file_size1(path);
  //std::this_thread::sleep_for(1s);
  //co_yield true;
  //L.info("Mid 2 %s", path.c_str());
  //co_yield true;
  //L.info("Mid 2 %s", path.c_str());
  //co_yield true;
  
  L.info("End 2 %s", path.c_str());
  co_return 4;
}

async<int> double_the_file_size(std::string path) {
  L.info("Start 1 %s", path.c_str());
  //auto p = co_await get_promise<async<int>::promise_type> {};
  
  auto size = co_await get_file_size(path);
  
  L.info("End 1 %s", path.c_str());
  co_return size * 2;
  co_await std::suspend_always {};
}

void aaaaaaaaaaaaaaaa() {
  static executor_t default_executor;
  
  //auto ret = wait_for(coro());
  auto r = default_executor.launch(double_the_file_size("A"));
  auto r1 = default_executor.launch(double_the_file_size("B"));
  
  r.future_.wait();
  printf("VALUE = %d\n", r.future_.get());
  
  //auto handle = coro();
  //async<void>::wait_for(handle);
  //int ret = p.value_;
}

#endif //CYD_UI_COROUTINES_H
