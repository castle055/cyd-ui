//
// Created by castle on 9/3/22.
//

#ifndef CYD_UI_PROPERTIES_H
#define CYD_UI_PROPERTIES_H


#include "../../../include/components.h"

class Property {
  
  std::vector<Property*>                          listeners;
  std::vector<cydui::components::ComponentState*> renderListeners;
protected:
  void* value = nullptr;
  std::function<void()> binding = []() { };


public:
  virtual ~Property();
  
  virtual void set_raw_value(void* val) = 0;
  
  void bindComponentRender(cydui::components::ComponentState* component_state);
  
  void addListener(Property* prop);
  
  void update();
};

#include "int_properties.h"

namespace cydui::properties {

}


#endif //CYD_UI_PROPERTIES_H
