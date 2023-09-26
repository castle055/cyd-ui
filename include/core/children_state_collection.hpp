//
// Created by castle on 9/11/22.
//

#ifndef CYD_UI_CHILDREN_STATE_COLLECTION_HPP
#define CYD_UI_CHILDREN_STATE_COLLECTION_HPP

#include <unordered_map>
#include <vector>

class ChildrenStateCollection {
  std::unordered_map<unsigned long, void*>              elements;
  std::unordered_map<unsigned long, std::vector<void*>> element_lists;

public:
  bool contains(unsigned long state_id, std::size_t index = 0);
  
  void* add(unsigned long state_id, void* state);
  
  void* add_list(unsigned long state_id, std::size_t index, void* state);
  
  void* get_list(unsigned long state_id, std::size_t index);
  
  void* operator[](unsigned long state_id);
};


#endif //CYD_UI_CHILDREN_STATE_COLLECTION_HPP
