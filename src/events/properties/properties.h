//
// Created by castle on 9/3/22.
//

#ifndef CYD_UI_PROPERTIES_H
#define CYD_UI_PROPERTIES_H

#include <functional>
#include "../events.h"
#include "../../logging/logging.h"
#include <unordered_set>

#define bind(state) on_change([state](){ state->_dirty = true; })

class Property {
  
  std::unordered_set<Property*>* dependencies;
  std::unordered_set<Property*>* listeners;
  
  bool dead      = false;
  bool _updating = false;
protected:
  void* value = nullptr;
  std::function<void()> binding = []() { };
  
  virtual bool is_equal(void* new_val) = 0;
  
  virtual bool changed_value() = 0;
  
  void clearDependencies();
  
  void clearListeners();

public:
  bool persistent = true; // Persistent after all listeners have unsubscribed
  explicit Property();
  
  virtual ~Property();
  
  void set_raw_value(void* val);
  
  void on_change(std::function<void()> function_listener);
  
  void addListener(Property* prop);
  
  void removeListener(Property* prop);
  
  void update();

private:
  std::vector<std::function<void()>>* function_listeners;
};

#include "int_properties.h"

namespace cydui::properties {

}


#endif //CYD_UI_PROPERTIES_H
