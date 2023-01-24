//
// Created by victor on 12/31/22.
//

#ifndef CYD_UI_CHANGE_EVENTS_H
#define CYD_UI_CHANGE_EVENTS_H

#include "events.hpp"
#include "cyd-log/dist/include/logging.hpp"

extern logging::logger chev_log;

namespace cydui::events::change_ev {
  template<typename T> requires EventType<T>
  class DataMonitor {
    typename T::DataType data;
    Event                                                                   * ev = nullptr;
  
  public:
    typedef std::function<bool(typename T::DataType, typename T::DataType)> change_predicate_t;
    
    explicit DataMonitor(change_predicate_t predicate) {
      this->predicate = predicate;
      this->ev        = new Event {
        .type = T::type,
        .ev = new T({.data = this->data}),
        .managed = true,
      };
      chev_log.debug("EMITTING event: (0x%X) %s", this->ev, this->ev->type.c_str());
      emit_raw(this->ev);
    }
    
    void update(typename T::DataType new_data) {
      if (predicate(this->data, new_data)) {
        this->ev->ev_mtx.lock();
        ((T*)this->ev->ev)->data = new_data;
        this->ev->ev_mtx.unlock();
        this->data = new_data;
        chev_log.debug("UPDATE. ev(0x%X) ev->status = %d", this->ev, this->ev->status);
        this->ev->ev_mtx.lock();
        if (this->ev->status == CONSUMED) {
          chev_log.debug("RE-EMITTING event: (0x%X) %s", this->ev, this->ev->type.c_str());
          this->ev->status = PENDING;
          this->ev->ev_mtx.unlock();
          emit_raw(this->ev);
        } else {
          this->ev->ev_mtx.unlock();
        }
      }
    }
  
  private:
    change_predicate_t predicate = [](typename T::DataType, typename T::DataType) { return true; };
  };
  
}

#endif //CYD_UI_CHANGE_EVENTS_H
