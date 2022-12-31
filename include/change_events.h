//
// Created by victor on 12/31/22.
//

#ifndef CYD_UI_CHANGE_EVENTS_H
#define CYD_UI_CHANGE_EVENTS_H

#include "events.hpp"

namespace cydui::events::change_ev {
    template<typename T>
    requires EventType<T>
    class DataMonitor {
        typename T::DataType data;
        Event* ev = nullptr;

    public:
        typedef std::function<bool(typename T::DataType, typename T::DataType)> change_predicate_t;
        explicit DataMonitor(change_predicate_t predicate) {
            this->predicate = predicate;
            this->ev = new Event {
                .type = T::type,
                .ev = new T({ .data = this->data }),
                .managed = true,
            };
            emit_raw(this->ev);
        }

        void update(typename T::DataType new_data){
            if (predicate(this->data, new_data)) {
                if (this->ev->status != CONSUMED) {
                    T* prev = (T*) this->ev->ev;
                    this->ev->ev = new T({ .data = new_data });
                    this->data = new_data;
                    delete prev;
                } else {
                    delete this->ev;
                    this->data = new_data;
                    this->ev = new Event {
                            .type = T::type,
                            .ev = new T({ .data = this->data }),
                            .managed = true,
                    };
                    emit_raw(this->ev);
                }
            }
        }
    private:
        change_predicate_t predicate = [](typename T::DataType, typename T::DataType){ return true; };
    };

}

#endif //CYD_UI_CHANGE_EVENTS_H
