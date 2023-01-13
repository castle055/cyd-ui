//
// Created by castle on 8/22/22.
//

#ifndef CYD_UI_THREADING_HPP
#define CYD_UI_THREADING_HPP

#include <thread>
#include <chrono>

namespace cydui::threading {
  struct thread_t {
    void* native_thread = nullptr;
    bool running = false;
    void* data = nullptr;
    
    thread_t* set_name(const std::string &name);
  };
  
  thread_t* new_thread(void(task)(thread_t* this_thread));
  
  thread_t* new_thread(void(task)(thread_t* this_thread), void* data);
}// namespace cydui::threading

namespace cydui::tasks {
  typedef unsigned long task_id;
  typedef unsigned long timer_id;
  
  class task_t {
    task_id    id;
    std::mutex task_mutex;
    
    int  progress = 0;
    bool running  = false;
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
  struct timer_t {
    std::chrono::duration<long, std::milli> period = 1s;
    int                                     count  = -1;
    
  };

#define SIMPLE_TASK(NAME, ARGS, BLOCK) \
  struct NAME##Args ARGS; \
  class NAME: public cydui::tasks::simple_task_t<NAME##Args> { \
    logging::logger log {.name = #NAME }; \
  protected: \
    void main() override BLOCK \
  };
  
  
  template<typename A>
  class simple_task_t: public tasks::task_t {
  protected:
    A args;
  public:
    void run(A _args) {
      this->args = _args;
      start(this);
    }
    
    void res() { }
  };

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
  protected:
    A args;
    R result;
    R default_result;
  public:
    void run(A _args) {
      this->args = _args;
      start(this);
    }
    
    R* res() {
      if (is_complete())
        return &result;
      else return &default_result;
    }
  };
  
  void start_thd();
  
  void start(task_t* task);
  
  void add_timer(timer_t timer);
}


#endif//CYD_UI_THREADING_HPP
