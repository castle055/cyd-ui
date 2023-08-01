//
// Created by castle on 8/1/23.
//

#ifndef CYD_UI_BATCH_ALLOCATOR_H
#define CYD_UI_BATCH_ALLOCATOR_H

#include <deque>

template<typename T, unsigned long BATCH_SIZE>
struct batch_t {
  T data[BATCH_SIZE] {};
  
  unsigned long used = 0;
  unsigned long removed = 0;
  
  bool is_full() {
    return used == BATCH_SIZE;
  }
  
  bool is_orphan() {
    return removed == used && used > 0;
  }
  
  bool contains(T* ptr) {
    return (unsigned char*) ptr > (unsigned char*) &data[0]
      && (unsigned char*) ptr <= (unsigned char*) &(data[BATCH_SIZE - 1]);
  }
  
  T* alloc() {
    T* t = &(data[used]);
    used++;
    return t;
  }
  
  void free(T* ptr) {
    removed++;
  }
};

// TODO - Get page size dynamically - that's what the 4096 is here
template<typename T, unsigned long BATCH_SIZE = (4UL * 1024UL) / sizeof(T)>
struct batch_allocator_t {
  using batch_ti = batch_t<T, BATCH_SIZE>;
  std::deque<batch_ti*> batches {};
  
  batch_ti* allocate_batch() {
    auto* t = new batch_ti;
    batches.push_back(t);
    return t;
  }
  
  T* alloc() {
    if (batches.empty()) {
      auto* b = allocate_batch();
      return b->alloc();
    }
    auto* back = batches.back();
    if (back->is_full()) {
      auto* b = allocate_batch();
      return b->alloc();
    } else {
      return back->alloc();
    }
  }
  
  void free(T* ptr) {
    if (batches.empty()) return;
    std::size_t i = 0;
    for (auto &b: batches) {
      if (b->contains(ptr)) {
        b->free(ptr);
        if (b->is_orphan()) {
          batches.erase(batches.begin() + i);
          delete b;
        }
        return;
      }
      ++i;
    }
  }
  
  void free_all() {
    while (!batches.empty()) {
      auto* b = batches.front();
      delete b;
      batches.pop_front();
    }
  }
  
  void merge_from(batch_allocator_t<T, BATCH_SIZE> &other) {
    if (this == &other) return;
    auto size = other.batches.size();
    for (std::size_t i = 0; i < size; ++i) {
      batches.push_back(other.batches.front());
      other.batches.pop_front();
    }
  }
};

#endif //CYD_UI_BATCH_ALLOCATOR_H
