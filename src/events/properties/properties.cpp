//
// Created by castle on 9/3/22.
//

#include "../../../include/properties.hpp"

static logging::logger log_lay = {.name = "PROPS"};

void Property::on_change(std::function<void()> function_listener) {
  this->function_listeners->push_back(function_listener);
}

void Property::addListener(Property* prop) {
  this->listeners->insert(prop);
  prop->dependencies->insert(this);
  
  //if (listeners->size() > 1000)
  //log_lay.debug("PROP(%X) %d LISTENERS", this, listeners->size());
  
}

void Property::removeListener(Property* prop) {
  if (listeners) {
    if (listeners->count(prop) > 0)
      listeners->erase(prop);
    
  }
  if (!persistent && (!listeners || listeners->empty()) && (!function_listeners || function_listeners->empty())) {
    //dead = true;
    delete this;
  }
}

void Property::_update_deps() {
  for (auto &item: *function_listeners) {
    item();
  }
  
  if (_updating)
    return;
  _updating = true;
  
  
  std::vector<Property*> l_dead;
  for (auto              &p: *listeners) {
    if (p->changed_value())
      p->_update_deps();
    if (p->dead)
      l_dead.push_back(p);
  }
  
  for (auto &item: l_dead) {
    delete item;
  }
  
  _updating = false;
}

void Property::update() {
  binding();
  
  _update_deps();
}

void Property::set_raw_value(void* val) {
  clearDependencies();
  if (is_equal(val))
    return;
  value = val;
  update();
}

Property::Property() {
  this->listeners          = new std::unordered_set<Property*>();
  this->dependencies       = new std::unordered_set<Property*>();
  this->function_listeners = new std::vector<std::function<void()>>();
  
}

Property::~Property() {
  value = nullptr;
  clearListeners();
  delete listeners;
  listeners = nullptr;
  delete function_listeners;
  function_listeners = nullptr;
  clearDependencies();
  delete dependencies;
  dependencies = nullptr;
}

void Property::clearDependencies() {
  for (auto &item: *dependencies) {
    if (item->dead == 0 && item->value != nullptr)
      item->removeListener(this);
  }
  this->dependencies->clear();
}

void Property::clearListeners() {
  for (auto &item: *listeners) {
    if (item->dead == 0 && item->value != nullptr)
      item->dependencies->erase(this);
  }
  listeners->clear();
}
