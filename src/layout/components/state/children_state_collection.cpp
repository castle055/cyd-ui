//
// Created by castle on 9/11/22.
//

#include "../../../../include/children_state_collection.hpp"

bool ChildrenStateCollection::contains(unsigned long state_id, std::size_t index) {
  return elements.contains(state_id) || (element_lists.contains(state_id) && element_lists[state_id].size() > index);
}

void* ChildrenStateCollection::add(unsigned long state_id, void* state) {
  elements[state_id] = state;
  return state;
}

void* ChildrenStateCollection::add_list(unsigned long state_id, std::size_t index, void* state) {
  if (index >= element_lists[state_id].size())
    element_lists[state_id].resize(index + 1);
  element_lists[state_id][index] = state;
  return state;
}

void* ChildrenStateCollection::get_list(unsigned long state_id, std::size_t index) {
  return element_lists[state_id][index];
}

void* ChildrenStateCollection::operator[](unsigned long state_id) {
  return elements[state_id];
}
