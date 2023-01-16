//
// Created by castle on 9/3/22.
//

#ifndef CYD_UI_PROPERTIES_HPP
#define CYD_UI_PROPERTIES_HPP

#include <functional>
#include "events.hpp"
#include "cyd-log/dist/include/logging.hpp"
#include <unordered_set>
#include <vector>

#define bind(state) on_change([state](){ state->_dirty = true; })

EVENT(UpdatePropEvent, {
  unsigned int win = 0;
  const void* target_property = nullptr;
  const void* new_value       = nullptr;
})

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
  
  void _update_deps();

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

#include "int_properties.hpp"

namespace cydui::properties {

}


#endif //CYD_UI_PROPERTIES_HPP
