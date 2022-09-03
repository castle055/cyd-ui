//
// Created by castle on 9/3/22.
//

#include "properties.h"


void Property::bindComponentRender(cydui::components::ComponentState* component_state) {
  this->renderListeners.push_back(component_state);
}

void Property::addListener(Property* prop) {
  this->listeners.push_back(prop);
}

void Property::update() {
  binding();
  
  for (const auto &item: listeners) {
    item->update();
  }
  
  for (auto &item: renderListeners) {
    item->_dirty = true;
  }
}

Property::~Property() = default;
