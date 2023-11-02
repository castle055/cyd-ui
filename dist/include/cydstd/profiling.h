//
// Created by castle on 10/19/23.
//

#ifndef CYD_UI_PROFILING_H
#define CYD_UI_PROFILING_H

#include <chrono>
#include <optional>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <cstring>
#include <utility>

using namespace std::chrono_literals;


namespace prof {
    constexpr std::size_t PROFILING_THREAD_NAME_SIZE = 32;
    constexpr std::size_t PROFILING_TIMELINE_SIZE = 10000;
    
    using PROFILING_CLOCK = std::chrono::steady_clock;
    
    using namespace std::chrono;
    
    using time_point = std::chrono::time_point<PROFILING_CLOCK>;
    using duration = std::chrono::duration<PROFILING_CLOCK>;
    
    struct event_t {
      const char* description;
      time_point t0;
      std::optional<time_point> t1 = std::nullopt;
      
      inline void mark_start() {
        t0 = PROFILING_CLOCK::now();
      }
      
      inline void mark() {
        mark_start();
      }
      
      inline void mark_end() {
        t1 = PROFILING_CLOCK::now();
      }
    };
    
    using thread_timeline_t = std::array<event_t, PROFILING_TIMELINE_SIZE>;
    
    struct thread_context_t {
      char name[PROFILING_THREAD_NAME_SIZE] {};
      
      inline void add_event(event_t ev) {
        timeline[insert_index] = ev;
        insert_index = insert_index + 1;
        if (insert_index >= PROFILING_TIMELINE_SIZE) {
          insert_index = 0;
        }
      }
      
      thread_timeline_t timeline {};
    
    private:
      std::size_t insert_index = 0;
    };
    
    struct context_t {
      inline void mark_event(const char* ev_desc) {
        event_t ev;
        ev.description = ev_desc;
        ev.mark();
        
        auto id = std::this_thread::get_id();
        add_thd(id);
        std::shared_lock lk {mtx};
        threads[id].add_event(ev);
      }
      
      inline event_t start_event(const char* ev_desc) {
        event_t ev;
        ev.description = ev_desc;
        ev.mark_start();
        auto id = std::this_thread::get_id();
        add_thd(id);
        return ev;
      }
      
      inline void end_event(event_t ev) {
        ev.mark_end();
        std::shared_lock lk {mtx};
        threads[std::this_thread::get_id()].add_event(ev);
      }
      
      struct scope_event_t {
        scope_event_t(
          context_t* ctx,
          const char* ev_desc
        ): ctx(ctx) {
          ev.description = ev_desc;
          ev.mark_start();
        }
        
        ~scope_event_t() {
          ev.mark_end();
          auto id = std::this_thread::get_id();
          std::shared_lock lk {ctx->mtx};
          ctx->threads[id].add_event(ev);
        }
      
      private:
        context_t* ctx;
        event_t ev {};
      };
      
      inline scope_event_t scope_event(const char* ev_desc) {
        auto id = std::this_thread::get_id();
        add_thd(id);
        return {this, ev_desc};
      }
      
      inline void add_thd(std::thread::id id) {
        std::unique_lock lk {mtx};
        if (!threads.contains(id)) {
          threads[id] = {};
        }
      }
      
      std::shared_mutex mtx;
      std::unordered_map<std::thread::id, thread_context_t> threads {};
    };
}

#endif //CYD_UI_PROFILING_H
