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

using namespace std::chrono_literals;


namespace prof {
    constexpr std::size_t PROFILING_EV_DESCRIPTION_SIZE = 32;
    constexpr std::size_t PROFILING_THREAD_NAME_SIZE = PROFILING_EV_DESCRIPTION_SIZE;
    constexpr std::size_t PROFILING_TIMELINE_SIZE = 500;
    
    using PROFILING_CLOCK = std::chrono::steady_clock;
    
    using namespace std::chrono;
    
    using time_point = std::chrono::time_point<PROFILING_CLOCK>;
    using duration = std::chrono::duration<PROFILING_CLOCK>;
    
    struct event_t {
      char description[PROFILING_EV_DESCRIPTION_SIZE] {};
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
        if (insert_index++ >= PROFILING_TIMELINE_SIZE) {
          insert_index = 0;
        }
      }
      
      thread_timeline_t timeline {};
    
    private:
      std::size_t insert_index = 0;
    };
    
    struct context_t {
      inline void mark_event(const char ev_desc[PROFILING_EV_DESCRIPTION_SIZE]) {
        event_t ev;
        memcpy(ev.description, ev_desc, PROFILING_EV_DESCRIPTION_SIZE);
        ev.mark();
        
        threads[std::this_thread::get_id()].add_event(ev);
      }
      
      inline event_t start_event(const char ev_desc[PROFILING_EV_DESCRIPTION_SIZE]) const {
        event_t ev;
        memcpy(ev.description, ev_desc, PROFILING_EV_DESCRIPTION_SIZE);
        ev.mark_start();
        return ev;
      }
      
      inline void end_event(event_t ev) {
        ev.mark_end();
        threads[std::this_thread::get_id()].add_event(ev);
      }
      
      struct scope_event_t {
        scope_event_t(
          context_t* ctx,
          std::string_view ev_desc
        ): ctx(ctx) {
          memcpy(ev.description, ev_desc.data(), std::min(ev_desc.size(), PROFILING_EV_DESCRIPTION_SIZE));
          ev.mark_start();
        }
        
        ~scope_event_t() {
          ev.mark_end();
          auto id = std::this_thread::get_id();
          if (!ctx->threads.contains(id)) {
            ctx->threads[id] = {};
          }
          ctx->threads[id].add_event(ev);
        }
      
      private:
        context_t* ctx;
        event_t ev {};
      };
      
      scope_event_t scope_event(std::string_view ev_desc) {
        return {this, ev_desc};
      }
      
      std::unordered_map<std::thread::id, thread_context_t> threads {};
    };
}

#endif //CYD_UI_PROFILING_H
