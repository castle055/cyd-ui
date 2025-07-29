//
// Created by castle on 2/18/25.
//
export module cydui.animations.AnimationOptions;

import std;
import fabric.logging;

export import cydui.easing_functions;

namespace cydui::animations {
  export struct AnimationOptions {
    using duration_t = std::chrono::system_clock::duration;

    AnimationOptions() = default;

    AnimationOptions& easing(easing::function_type fun) {
      easing_function_ = fun;
      return *this;
    }

    AnimationOptions& duration(duration_t d) {
      duration_ = d;
      return *this;
    }

    easing::function_type easing_function_{easing::linear};
    duration_t            duration_;
  };
} // namespace cydui::animations
