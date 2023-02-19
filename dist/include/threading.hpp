//
// Created by castle on 8/22/22.
//

#ifndef CYD_UI_THREADING_HPP
#define CYD_UI_THREADING_HPP

#include <thread>
#include <chrono>
#include "cydstd/cydstd.h"

namespace cydui::threading {
    struct thread_t {
      void* native_thread = nullptr;
      bool running = false;
      void* data = nullptr;
      
      thread_t* set_name(const str &name);
    };
    
    thread_t* new_thread(void(task)(thread_t* this_thread));
    
    thread_t* new_thread(void(task)(thread_t* this_thread), void* data);
}// namespace cydui::threading

namespace cydui::tasks {
    typedef unsigned long task_id;
    typedef unsigned long timer_id;
    
    class task_t {
      task_id id;
      std::mutex task_mutex;
      
      int progress = 0;
      bool running = false;
    protected:
      virtual void main() = 0;
      
      void set_progress(int p);
    
    public:
      bool exec();
      
      task_id get_id();
      
      bool reset();
      
      int get_progress();
      
      bool is_complete();
    };
    
    using namespace std::chrono_literals;
    struct _timer_t {
      std::chrono::duration<long, std::nano> last_time = std::chrono::system_clock::now().time_since_epoch();
      std::chrono::duration<long, std::nano> period = 1s;
      int count = -1;
      bool run_now = false;
      
      task_t* task = nullptr;
    };
    
    struct NoResult { };

#define SIMPLE_TASK_W_RESULT(NAME, ARGS, RESULT, BLOCK) \
  struct NAME##Args ARGS; \
  struct NAME##Result RESULT; \
  class NAME: public cydui::tasks::simple_task_w_result_t<NAME##Args, NAME##Result> { \
    logging::logger log {.name = #NAME }; \
  protected: \
    void main() override BLOCK \
  };

#define TASK_W_RESULT(NAME, ARGS, RESULT, BLOCK) \
  struct NAME##Args ARGS; \
  struct NAME##Result RESULT; \
  class NAME: public cydui::tasks::simple_task_w_result_t<NAME##Args, NAME##Result> { \
    logging::logger log {.name = #NAME }; \
  protected: \
    void main() override BLOCK                   \
  private:
    
    template<typename A, typename R>
    class simple_task_w_result_t: public tasks::task_t {
    public:
      typedef A Args;
      typedef R Result;
      
      //simple_task_w_result_t() = default;
      //simple_task_w_result_t(const simple_task_w_result_t<A,R>&) = default;
    protected:
      Args args;
      Result result;
      Result default_result;
    
    public:
      void run(Args _args) {
        this->args = _args;
        start(this);
      }
      
      void main() override {
      
      }
      
      Result* res() {
        if (is_complete())
          return &result;
        else return &default_result;
      }
      
      void set_args(Args _args) {
        this->args = _args;
      }
    };
    
    template<typename A>
    class simple_t: public simple_task_w_result_t<A, NoResult> { };

#define SIMPLE_TASK(NAME, ARGS, BLOCK) \
  struct NAME##Args ARGS; \
  class NAME: public cydui::tasks::simple_t<NAME##Args> { \
    logging::logger log {.name = #NAME }; \
  protected: \
    void main() override BLOCK \
  };
    
    void start_thd();
    
    void start(task_t* task);
    
    void start_timer(_timer_t* timer);
    
    template<typename T>
    concept TaskType = requires {
      typename T::Args;
      typename T::Result;
      //{ T() } -> std::convertible_to<simple_task_w_result_t<typename T::Args, typename T::Result>>;
    };
    
    template<typename T> requires TaskType<T>
    class timer_t: public tasks::_timer_t {
      T _task;
    public:
      void start(
        typename T::Args _args,
        std::chrono::duration<long, std::nano> period,
        int count = -1,
        bool run_now = false
      ) {
        this->period = period;
        this->count = count;
        this->run_now = run_now;
        this->last_time = std::chrono::system_clock::now().time_since_epoch();
        _task.set_args(_args);
        this->task = &_task;
        start_timer(this);
      };
      
      typename T::Result* res() {
        return _task.res();
      }
    };
  
}


#endif//CYD_UI_THREADING_HPP
