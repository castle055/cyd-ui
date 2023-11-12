//
// Created by victor on 12/31/22.
//

#ifndef CYD_UI_CHANGE_EVENTS_H
#define CYD_UI_CHANGE_EVENTS_H

#include "events.hpp"
#include "../cydstd/logging.hpp"

extern logging::logger chev_log;

namespace cydui::events::change_ev {
    template<EventType T>
    class DataMonitor {
      Event* ev = nullptr;
    
    public:
      typedef std::function<bool(typename T::DataType &, typename T::DataType &)> change_predicate_t;
      typedef std::function<void(typename T::DataType &)> completion_callback_t;
      
      DataMonitor(
        change_predicate_t predicate,
        completion_callback_t completion_callback = [](typename T::DataType &) { }
      ) {
        this->predicate = predicate;
        this->completion_callback = completion_callback;
        this->ev = new Event {
          .type = T::type,
          .ev = new T,
          .managed = true,
        };
        chev_log.debug("EMITTING event: (0x%lX) %s", (unsigned long) this->ev, this->ev->type.c_str());
        emit_raw(this->ev);
      }
      
      void update(typename T::DataType new_data) {
        this->ev->ev_mtx.lock();
        if (this->ev->status == CONSUMED) {
          this->ev->ev_mtx.unlock();
          completion_callback(((T*) this->ev->ev)->data);
        } else {
          this->ev->ev_mtx.unlock();
        }
        
        if (predicate(((T*) this->ev->ev)->data, new_data)) {
          this->ev->ev_mtx.lock();
          ((T*) this->ev->ev)->data = new_data;
          this->ev->ev_mtx.unlock();
          chev_log.debug("UPDATE. ev(0x%lX)[%s] ev->status = %d", (unsigned long) this->ev, this->ev->type.c_str(),
            this->ev->status);
          this->ev->ev_mtx.lock();
          if (this->ev->status == CONSUMED) {
            chev_log.debug("RE-EMITTING event: (0x%lX) %s", (unsigned long) this->ev, this->ev->type.c_str());
            this->ev->status = PENDING;
            this->ev->ev_mtx.unlock();
            emit_raw(this->ev);
          } else {
            this->ev->ev_mtx.unlock();
          }
        }
      }
    
    private:
      completion_callback_t completion_callback;
      change_predicate_t predicate = [](typename T::DataType &, typename T::DataType &) {
        return true;
      };
    };
  
}

#endif //CYD_UI_CHANGE_EVENTS_H
