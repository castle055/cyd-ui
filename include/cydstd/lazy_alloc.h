//
// Created by castle on 10/26/23.
//

#ifndef CYD_UI_LAZY_ALLOC_H
#define CYD_UI_LAZY_ALLOC_H

#include <functional>

template<typename T, typename ...Args>
struct lazy_alloc {
  explicit lazy_alloc(Args... args) {
    init = [args...](lazy_alloc &self) {
      self.val = new T {args...};
    };
  }
  ~lazy_alloc() {
    delete val;
  }
  
  T* operator->() {
    if (nullptr == val) {
      init(*this);
    }
    return val;
  }
  
  operator T*() {
    if (nullptr == val) {
      init(*this);
    }
    return val;
  }
private:
  std::function<void(lazy_alloc &)> init = [](lazy_alloc &self) {
    self.val = new T {};
  };
  T* val = nullptr;
};

#endif //CYD_UI_LAZY_ALLOC_H
