/*! \file  SwapPtr.cppm
 *! \brief
 *!
 */

export module cydui.platform.render.cairo.SwapPtr;

import std;
import reflect;

export template <typename T>
class SwapPtr {
  std::unique_ptr<T> front_;
  std::unique_ptr<T> back_;

public:
  template <typename... Args>
  explicit SwapPtr(Args&&... args)
      : front_(std::make_unique<T>(std::forward<Args>(args)...)),
        back_(std::make_unique<T>(std::forward<Args>(args)...)) {}

  T& get_front() {
    return *front_;
  }

  T& get_back() {
    return *back_;
  }

  void swap() {
    std::swap(front_, back_);
  }
};
