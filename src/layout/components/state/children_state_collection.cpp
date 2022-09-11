//
// Created by castle on 9/11/22.
//

#include "children_state_collection.h"

bool ChildrenStateCollection::contains(unsigned long state_id) {
  return elements.contains(state_id);
}

void* ChildrenStateCollection::add(unsigned long state_id, void* state) {
  elements[state_id] = state;
  return state;
}

void* ChildrenStateCollection::operator[](unsigned long state_id) {
  return elements[state_id];
}
