//
// Created by castle on 7/18/23.
//

#ifndef CYDONIA_DEQUE_H
#define CYDONIA_DEQUE_H

#include <mutex>

#include "batch_allocator.h"

template<typename T>
struct deque_node_t: public T {
  // 16 bytes
  deque_node_t* prev = nullptr;
  deque_node_t* next = nullptr;
  //
  
  deque_node_t<T>* operator++() {
    return next;
  }
  
  bool operator!=(const deque_node_t<T>* other) const {
    return this != other;
  }
};

enum ThreadSafety {
  NO_THREAD_SAFE,
  THREAD_SAFE,
};

template<typename T, ThreadSafety threadSafety = NO_THREAD_SAFE>
struct deque_t {
private:
  std::mutex mtx;
  
  unsigned long _size = 0UL;
  
  deque_node_t<T>* head = nullptr;
  deque_node_t<T>* last = nullptr;
  
  batch_allocator_t<deque_node_t<T>> batch_allocator;
public:
  deque_t() = default;
  
  deque_t(std::initializer_list<T> values) {
    if (threadSafety == THREAD_SAFE) std::scoped_lock lock(mtx);
    for (const auto &item: values) {
      push_back(item);
    }
  }
  
  bool empty() {
    if (threadSafety == THREAD_SAFE) std::scoped_lock lock(mtx);
    return _size == 0UL;
  }
  
  void push_front(const T &val) {
    if (threadSafety == THREAD_SAFE) std::scoped_lock lock(mtx);
    //auto* node = new deque_node_t<T> {val};
    auto* node = batch_allocator.alloc();
    *node = deque_node_t<T> {val};
    
    if (empty()) {
      head = node;
      last = node;
    } else {
      node->next = head;
      head->prev = node;
      head = node;
    }
    _size++;
  }
  
  void push_back(const T &val) {
    if (threadSafety == THREAD_SAFE) std::scoped_lock lock(mtx);
    //auto* node = new deque_node_t<T> {val};
    auto* node = batch_allocator.alloc();
    *node = deque_node_t<T> {val};
    
    if (empty()) {
      head = node;
      last = node;
    } else {
      node->prev = last;
      last->next = node;
      last = node;
    }
    _size++;
  }
  
  deque_node_t<T>* peek_front() {
    if (threadSafety == THREAD_SAFE) std::scoped_lock lock(mtx);
    return empty() ? nullptr : head;
  }
  
  deque_node_t<T>* peek_back() {
    if (threadSafety == THREAD_SAFE) std::scoped_lock lock(mtx);
    return empty() ? nullptr : last;
  }
  
  T pop_front() {
    if (threadSafety == THREAD_SAFE) std::scoped_lock lock(mtx);
    if (empty()) {
      return T {};
    } else {
      T val = head;
      
      auto* tmp = head;
      
      if (head->next) {
        head->next->prev = nullptr;
        head = head->next;
      } else {
        head = nullptr;
        last = nullptr;
      }
      
      batch_allocator.free(tmp);
      //delete tmp;
      _size--;
      return val;
    }
  }
  
  T pop_back() {
    if (threadSafety == THREAD_SAFE) std::scoped_lock lock(mtx);
    if (empty()) {
      return T {};
    } else {
      T val = *last;
      
      auto* tmp = last;
      
      if (last->prev) {
        last->prev->next = nullptr;
        last = last->prev;
      } else {
        head = nullptr;
        last = nullptr;
      }
      
      batch_allocator.free(tmp);
      //delete tmp;
      _size--;
      return val;
    }
  }
  
  deque_node_t<T>* operator[](unsigned long index) {
    if (threadSafety == THREAD_SAFE) std::scoped_lock lock(mtx);
    if (empty()) return nullptr;
    
    auto* node = head;
    unsigned long i = 0;
    for (; i < index, node != last; ++i, node = node->next);
    
    if (i == index) {
      return node;
    } else {
      return nullptr;
    }
  }
  
  void insert_ahead(deque_node_t<T>* at, const T &val) {
    if (threadSafety == THREAD_SAFE) std::scoped_lock lock(mtx);
    if (at == nullptr || empty()) {
      return;
    } else if (empty()) {
      push_front(val);
    } else if (at == last) {
      push_back(val);
    } else {
      //auto* node = new deque_node_t<T> {.data = val};
      deque_node_t<T>* node = batch_allocator.alloc();
      *node = deque_node_t<T> {val};
      
      node->prev = at;
      node->next = at->next;
      at->next->prev = node;
      at->next = node;
      
      _size++;
    }
  }
  
  T remove(deque_node_t<T>* at) {
    if (threadSafety == THREAD_SAFE) std::scoped_lock lock(mtx);
    if (empty()) return T {};
    if (at == last) {
      return pop_back();
    } else if (at == head) {
      return pop_front();
    } else {
      T val = at;
      at->next->prev = at->prev;
      at->prev->next = at->next;
      batch_allocator.free(at);
      //delete at;
      _size--;
      return val;
    }
  }
  
  [[nodiscard]] unsigned long len() const {
    if (threadSafety == THREAD_SAFE) std::scoped_lock lock(mtx);
    return _size;
  }
  
  deque_node_t<T>* begin() {
    if (threadSafety == THREAD_SAFE) std::scoped_lock lock(mtx);
    return head;
  }
  
  deque_node_t<T>* end() {
    if (threadSafety == THREAD_SAFE) std::scoped_lock lock(mtx);
    return last;
  }
  
  void append(deque_t<T, threadSafety> &other) {
    if (threadSafety == THREAD_SAFE) std::scoped_lock lock(mtx);
    if (other.empty()) return;
    if (empty()) {
      this->head = other.head;
      this->last = other.last;
    } else {
      this->last->next = other.head;
      other.head->prev = this->last;
      this->last = other.last;
    }
    this->_size += other._size;
    other._size = 0;
    other.head = nullptr;
    other.last = nullptr;
    batch_allocator.merge_from(other.batch_allocator);
  }
  
  unsigned long size() {
    return this->_size;
  }
  
  void clear() {
    if (threadSafety == THREAD_SAFE) std::scoped_lock lock(mtx);
    if (empty()) {
      return;
    } else {
      batch_allocator.free_all();
      head = nullptr;
      last = nullptr;
      _size = 0UL;
    }
  }
};

//struct test_t {
//  int x = 0, y = 0;
//};
//
//void tes() {
//  deque_t<test_t, THREAD_SAFE> pts {
//    {1, 3},
//    {4, 8},
//  };
//
//  for (const auto &item: pts) {
//
//  }
//
//  for (auto& point: pts) {
//    point.x;
//
//  }
//  auto* p = pts.peek_front();
//  p->x;
//  p->x;
//  pts[2]->x;
//}
//
#endif //CYDONIA_DEQUE_H
