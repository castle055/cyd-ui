//
// Created by castle on 8/22/22.
//

#include "../../include/events.hpp"
#include "../graphics/events.hpp"
#include "../../include/threading.hpp"
#include <thread>
#include <unordered_map>
#include <deque>
#include <vector>

using namespace cydui::threading;
using namespace std::chrono_literals;

logging::logger logger =
                  {.name = "TASKRUNNER", .on = true};

thread_t* cydui::threading::new_thread(void(task)(thread_t* this_thread)) {
  return new_thread(task, nullptr);
}

thread_t* cydui::threading::new_thread(
  void(task)(thread_t* this_thread), void* data
) {
  auto* arg = new thread_t();
  arg->running = true;
  arg->data    = data;
  auto* thread = new std::thread(task, arg);
  arg->native_thread = thread;
  return arg;
}


thread_t* cydui::threading::thread_t::set_name(const std::string &name) {
  pthread_t pt = ((std::thread*)native_thread)->native_handle();
  pthread_setname_np(pt, name.c_str());
  return this;
}


// ===== TASKS
std::mutex                                   task_queue_mtx;
std::mutex                                   timer_list_mtx;
typedef std::vector<cydui::tasks::_timer_t*> timer_list_t;
typedef std::deque<cydui::tasks::task_t*>    task_queue_t;

static timer_list_t timer_list;
static task_queue_t task_queue;

static void add_timer(cydui::tasks::_timer_t* timer) {
  timer_list_mtx.lock();
  if (std::find(timer_list.begin(), timer_list.end(), timer) == timer_list.end()) {
    timer_list.push_back(timer);
  }
  timer_list_mtx.unlock();
}

cydui::tasks::_timer_t* remove_timer(cydui::tasks::_timer_t* timer) {
  timer_list_mtx.lock();
  auto t = std::find(timer_list.begin(), timer_list.end(), timer);
  cydui::tasks::_timer_t* t_ret = *t;
  if (t != timer_list.end()) {
    timer_list.erase(t);
  } else t_ret = nullptr;
  timer_list_mtx.unlock();
  return t_ret;
}

static void push_task(cydui::tasks::task_t* task) {
  task_queue_mtx.lock();
  task_queue.push_back(task);
  task_queue_mtx.unlock();
}

static cydui::tasks::task_t* pop_task() {
  task_queue_mtx.lock();
  cydui::tasks::task_t* id = nullptr;
  if (!task_queue.empty()) {
    id = task_queue.front();
    task_queue.pop_front();
  }
  task_queue_mtx.unlock();
  return id;
}

static void task_executor(cydui::tasks::task_t* task) {
  if (task->reset()) {
    task->exec();
    // TODO - There should be a specific event for task completion
    cydui::events::emit<RedrawEvent>({ });
  }
}

static void trigger_executor() {
  timer_list_mtx.lock();
  for (auto &item: timer_list) {
    auto now = std::chrono::system_clock::now().time_since_epoch();
    auto* timer = item;
    if (!timer) continue;
    auto* task = item->task;
    
    //logger.debug("Found one count=%d, task=%X", timer->count, task);
    if (timer->count != 0 && task && (timer->run_now || now - timer->last_time > timer->period)) {
      timer->last_time = now;
      timer->run_now   = false;
      if (timer->count > 0) timer->count--;
      timer_list_mtx.unlock(); // NOTE - this is on purpose
      task_executor(task);
      timer_list_mtx.lock();
    }
  }
  timer_list_mtx.unlock();
}

static void taskrunner_task(thread_t* this_thread) {
  while (this_thread->running) {
    auto task = pop_task();
    if (task) {
      task_executor(task);
    }
    
    trigger_executor();
    
    std::this_thread::sleep_for(std::chrono::duration<int, std::nano>(10ms));
  }
}

cydui::threading::thread_t* taskrunner_thread;
struct thread_data {
  
};
thread_data* taskrunner_th_data = new thread_data;

void cydui::tasks::start_thd() {
  if (taskrunner_thread && taskrunner_thread->native_thread != nullptr)
    return;
  logger.debug("Starting taskrunner");
  
  taskrunner_thread =
    threading::new_thread(&taskrunner_task, taskrunner_th_data)
      ->set_name("TASKRUNNER_THD");
}

void cydui::tasks::start(task_t* task) {
  start_thd();
  push_task(task);
}

void cydui::tasks::start_timer(_timer_t* timer) {
  start_thd();
  add_timer(timer);
}


// TYPE IMPLEMENTATION

cydui::tasks::task_id cydui::tasks::task_t::get_id() {
  return this->id;
}

bool cydui::tasks::task_t::exec() {
  this->task_mutex.lock();
  if (!this->running) {
    this->progress = 1;
    this->running  = true;
    this->task_mutex.unlock();
    this->main();
    this->task_mutex.lock();
    this->progress = 100;
    this->running  = false;
    this->task_mutex.unlock();
    return true;
  } else {
    this->task_mutex.unlock();
    return false;
  }
}

bool cydui::tasks::task_t::reset() {
  this->task_mutex.lock();
  if (!this->running) {
    this->progress = 0;
    this->running  = false;
    this->task_mutex.unlock();
    return true;
  } else {
    this->task_mutex.unlock();
    return false;
  }
}

void cydui::tasks::task_t::set_progress(int p) {
  this->task_mutex.lock();
  this->progress = p;
  this->task_mutex.unlock();
}

int cydui::tasks::task_t::get_progress() {
  this->task_mutex.lock();
  int p = this->progress;
  this->task_mutex.unlock();
  return p;
}

bool cydui::tasks::task_t::is_complete() {
  this->task_mutex.lock();
  int p = this->progress;
  int r = this->running;
  this->task_mutex.unlock();
  return p > 0 && !r;
}
