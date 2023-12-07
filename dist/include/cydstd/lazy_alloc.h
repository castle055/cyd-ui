//
// Created by castle on 10/26/23.
//

#ifndef CYD_UI_LAZY_ALLOC_H
#define CYD_UI_LAZY_ALLOC_H

#include <functional>
#include <memory>

template<typename T, typename ...Args>
struct lazy_alloc {
  explicit lazy_alloc(Args... args) {
    init = [args...](lazy_alloc<T>* self) {
      self->val = std::make_shared<T>(args...);
    };
  }
  ~lazy_alloc() = default;
  
  T* operator->() {
    if (nullptr == val) {
      init(this);
    }
    return val.get();
  }
  
  operator T*() {
    if (nullptr == val) {
      init(this);
    }
    return val.get();
  }
  
  std::function<void(lazy_alloc<T>*)> init = [](lazy_alloc<T>* self) {
    self->val = std::make_shared<T>();
  };
  std::shared_ptr<T> val = nullptr;
};

#endif //CYD_UI_LAZY_ALLOC_H
