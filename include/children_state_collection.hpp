//
// Created by castle on 9/11/22.
//

#ifndef CYD_UI_CHILDREN_STATE_COLLECTION_HPP
#define CYD_UI_CHILDREN_STATE_COLLECTION_HPP

#include <unordered_map>

class ChildrenStateCollection {
  std::unordered_map<unsigned long, void*> elements;

public:
  bool contains(unsigned long state_id);
  
  void* add(unsigned long state_id, void* state);
  
  void* operator[](unsigned long state_id);
};


#endif //CYD_UI_CHILDREN_STATE_COLLECTION_HPP
