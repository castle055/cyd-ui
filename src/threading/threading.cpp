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

struct task_row_t {
  cydui::tasks::task_t* task;
};
struct timer_row_t {
  std::chrono::duration<long, std::nano> lasttime = 0s;
  cydui::tasks::timer_t* timer;
};

typedef std::unordered_map<cydui::tasks::timer_id, timer_row_t>           timer_table_t;
typedef std::unordered_map<cydui::tasks::timer_id, cydui::tasks::task_t*> trigger_table_t;

std::mutex                                task_queue_mtx;
typedef std::deque<cydui::tasks::task_t*> task_queue_t;

static timer_table_t   timer_table;
static trigger_table_t trigger_table;

// TASK QUEUE
static task_queue_t task_queue;

void push_task(cydui::tasks::task_t* task) {
  task_queue_mtx.lock();
  task_queue.push_back(task);
  task_queue_mtx.unlock();
}

cydui::tasks::task_t* pop_task() {
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
  std::vector<cydui::tasks::timer_id> to_delete;
  for (auto                           &item: trigger_table) {
    auto now = std::chrono::system_clock::now().time_since_epoch();
    if (timer_table.contains(item.first)) {
      auto timer_row = timer_table[item.first];
      auto* timer = timer_row.timer;
      auto* task  = item.second;
      
      if (now - timer_row.lasttime > timer->period && timer->count != 0) {
        timer_row.lasttime = now;
        if (timer->count > 0) timer->count--;
        task_executor(task);
      }
    } else {
      to_delete.push_back(item.first);
    }
  }
  for (const auto &item: to_delete) {
    trigger_table.erase(item);
  }
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
