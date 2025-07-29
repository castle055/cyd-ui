//
// Created by castle on 2/18/25.
//
export module cydui.animations.AnimationHandle;

import std;
import reflect;

import fabric.logging;
import fabric.async;
import fabric.wiring.signals;

namespace cydui::animations {
  export class AnimationHandle {
    std::function<void()> stop_callback_;

  public:
    AnimationHandle(const AnimationHandle& other)
        : stop_callback_(other.stop_callback_) {}

    explicit AnimationHandle(auto&& stop_callback)
        : stop_callback_(stop_callback) {}

    void stop() {
      stop_callback_();
    }
  };
} // namespace cydui::animations
