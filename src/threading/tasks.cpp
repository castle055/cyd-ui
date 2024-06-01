// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

#include "cyd_ui/tools/tasks.h"
#ifndef CYD_UI_TASKS_H

#include <deque>
#include <vector>

using namespace cyd::ui::threading;
using namespace std::chrono_literals;

std::mutex task_queue_mtx;
std::mutex timer_list_mtx;
typedef std::vector<cyd::ui::tasks::_timer_t*> timer_list_t;
typedef std::deque<cyd::ui::tasks::task_t*> task_queue_t;

static timer_list_t timer_list;
static task_queue_t task_queue;

static void add_timer(cyd::ui::tasks::_timer_t* timer) {
  timer_list_mtx.lock();
  if (std::find(timer_list.begin(), timer_list.end(), timer) == timer_list.end()) {
    timer_list.push_back(timer);
  }
  timer_list_mtx.unlock();
}

cyd::ui::tasks::_timer_t* remove_timer(cyd::ui::tasks::_timer_t* timer) {
  timer_list_mtx.lock();
  auto t = std::find(timer_list.begin(), timer_list.end(), timer);
  cyd::ui::tasks::_timer_t* t_ret = *t;
  if (t != timer_list.end()) {
    timer_list.erase(t);
  } else t_ret = nullptr;
  timer_list_mtx.unlock();
  return t_ret;
}

static void push_task(cyd::ui::tasks::task_t* task) {
  task_queue_mtx.lock();
  task_queue.push_back(task);
  task_queue_mtx.unlock();
}

static cyd::ui::tasks::task_t* pop_task() {
  task_queue_mtx.lock();
  cyd::ui::tasks::task_t* id = nullptr;
  if (!task_queue.empty()) {
    id = task_queue.front();
    task_queue.pop_front();
  }
  task_queue_mtx.unlock();
  return id;
}

static void task_executor(cyd::ui::tasks::task_t* task) {
  if (task->reset()) {
    task->exec();
    // TODO - There should be a specific event for task completion
    cyd::ui::async::emit<RedrawEvent>({});
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
      timer->run_now = false;
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

cyd::ui::threading::thread_t* taskrunner_thread;
struct thread_data {

};
thread_data* taskrunner_th_data = new thread_data;

void cyd::ui::tasks::start_thd() {
  if (taskrunner_thread && taskrunner_thread->native_thread != nullptr)
    return;
  logger.debug("Starting taskrunner");
  
  taskrunner_thread =
    threading::new_thread(&taskrunner_task, taskrunner_th_data)
      ->set_name("TASKRUNNER_THD");
}

void cyd::ui::tasks::start(task_t* task) {
  start_thd();
  push_task(task);
}

void cyd::ui::tasks::start_timer(_timer_t* timer) {
  start_thd();
  add_timer(timer);
}


// TYPE IMPLEMENTATION

cyd::ui::tasks::task_id cyd::ui::tasks::task_t::get_id() {
  return this->id;
}

bool cyd::ui::tasks::task_t::exec() {
  this->task_mutex.lock();
  if (!this->running) {
    this->progress = 1;
    this->running = true;
    this->task_mutex.unlock();
    this->main();
    this->task_mutex.lock();
    this->progress = 100;
    this->running = false;
    this->task_mutex.unlock();
    return true;
  } else {
    this->task_mutex.unlock();
    return false;
  }
}

bool cyd::ui::tasks::task_t::reset() {
  this->task_mutex.lock();
  if (!this->running) {
    this->progress = 0;
    this->running = false;
    this->task_mutex.unlock();
    return true;
  } else {
    this->task_mutex.unlock();
    return false;
  }
}

void cyd::ui::tasks::task_t::set_progress(int p) {
  this->task_mutex.lock();
  this->progress = p;
  this->task_mutex.unlock();
  cyd::ui::async::emit<RedrawEvent>({});
}

int cyd::ui::tasks::task_t::get_progress() {
  this->task_mutex.lock();
  int p = this->progress;
  this->task_mutex.unlock();
  return p;
}

bool cyd::ui::tasks::task_t::is_running() {
  this->task_mutex.lock();
  int r = this->running;
  this->task_mutex.unlock();
  return r;
}

bool cyd::ui::tasks::task_t::is_complete() {
  this->task_mutex.lock();
  int p = this->progress;
  int r = this->running;
  this->task_mutex.unlock();
  return p > 0 && !r;
}

#endif //CYD_UI_TASKS_H