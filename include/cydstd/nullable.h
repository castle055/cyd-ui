//
// Created by castle on 2/18/23.
//

#ifndef CYD_UI_NULLABLE_H
#define CYD_UI_NULLABLE_H

#include <functional>
#include <stdexcept>

template<typename F, typename T, typename R>
concept CCCC = requires(T t) {
  { F(t) } -> std::convertible_to<R>;
};

template<typename T>
class nullable {
  T* value = nullptr;
public:
  explicit operator bool() const {
    return value != nullptr;
  }
  
  nullable() = default;
  ~nullable() {
    if (value != nullptr) {
      delete value;
      value == nullptr;
    }
  }
  
  nullable(T& t) {
    value = new T(t);
  }
  
  nullable(const T& t) {
    value = new T(t);
  }
  
  nullable(T* t) {
    value = t;
  }
  
  nullable(const T* t) {
    value = t;
  }
  
  nullable<T> &operator=(T new_val) {
    if (value == nullptr) {
      value = new T(new_val);
    } else {
      *value = new_val;
    }
    return *this;
  }
  
  void set_null() {
    if (value != nullptr) {
      delete value;
      value = nullptr;
    }
  }
  
  template<typename R>
  nullable<R> let(const std::function<R(T)>& block) {
    if (value != nullptr) {
      return block(*value);
    }
    return nullable<R>();
  }
  
  template<typename R>
  nullable<R> let(const std::function<nullable<R>(T)>& block) {
    if (value != nullptr) {
      return block(*value);
    }
    return nullable<R>();
  }
  
  template<typename R>
  nullable<R> let(const std::function<R(T&)>& block) {
    if (value != nullptr) {
      return block(*value);
    }
    return nullable<R>();
  }
  
  template<typename R>
  nullable<R> let(const std::function<nullable<R>(T&)>& block) {
    if (value != nullptr) {
      return block(*value);
    }
    return nullable<R>();
  }
  
  template<typename R>
  nullable<R> let(const std::function<R(const T&)>& block) const {
    if (value != nullptr) {
      return block(*value);
    }
    return nullable<R>();
  }
  
  template<typename R>
  nullable<R> let(const std::function<nullable<R>(const T&)>& block) const {
    if (value != nullptr) {
      return block(*value);
    }
    return nullable<R>();
  }
  
  
  nullable<T> let(const std::function<void(T)>& block) {
    if (value != nullptr) {
      block(*value);
    }
    return nullable<T>();
  }
  
  nullable<T> let(const std::function<nullable<T>(T)>& block) {
    if (value != nullptr) {
      block(*value);
    }
    return nullable<T>();
  }
  
  nullable<T> let(const std::function<void(T&)>& block) {
    if (value != nullptr) {
      block(*value);
    }
    return nullable<T>();
  }
  
  nullable<T> let(const std::function<void(const T&)>& block) const {
    if (value != nullptr) {
      block(*value);
    }
    return nullable<T>();
  }
  
  //void let(const std::function<void(T&)>& block) {
  //  if (value != nullptr) {
  //    return block(*value);
  //  }
  //}
  //
  //void let(const std::function<void(const T&)>& block) const {
  //  if (value != nullptr) {
  //    return block(*value);
  //  }
  //}
  //
  nullable<T>& also(const std::function<void(T&)>& block) {
    if (value != nullptr) {
      block(*value);
    }
    return *this;
  }
  
  T operator or(T other) {
    if (value != nullptr) {
      return *value;
    } else {
      return other;
    }
  }
  
  void operator or(const std::function<void()>& block) {
    if (value == nullptr) {
      block();
    }
  }
  
  nullable<T> operator or(const std::function<T()>& block) const {
    if (value == nullptr) {
      return block();
    }
    return this;
  }
  
  T* unwrap() const {
    if (value == nullptr) {
      throw std::out_of_range("Nullable was null when it was not supposed to!");
    } else {
      return value;
    }
  }
  
};

#define _(ARG, BLOCK) (std::function([&](ARG it) {BLOCK} ))
#define __(ARG, BLOCK) (std::function([&]ARG {BLOCK} ))

//#define lm std::function([&]
//#define lma(A) std::function([&](A it)
//#define ml )
//
//static void iiii() {
//  _(int, {});
//  lm {} ml;
//
//  lma(int) {} ml (4);
//}
//
//static nullable<int> testt;
//
//static void ttt() {
//  testt = 4;
//  testt.set_null();
//
//  str aaa = testt
//    .let  _(int, { return (long)it * 2; })
//    .let  _(long, { return str(" "); })
//    .also _(str, { str(" "); })
//    .let  _(str, { return " "; }) or "asdf";
//}
//
#endif //CYD_UI_NULLABLE_H
