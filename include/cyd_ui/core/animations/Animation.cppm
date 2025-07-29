//
// Created by castle on 2/18/25.
//
export module cydui.animations.Animation;

import std;

import fabric.logging;
import fabric.async;
import fabric.wiring.signals;
export import cydui.easing_functions;
export import cydui.interpolation;
export import cydui.animations.complexity;
export import cydui.animations.AnimationOptions;
export import cydui.animations.Keyframe;

using namespace std::chrono_literals;

namespace cydui::animations {
  export class Animation {
    std::vector<std::pair<double, Keyframe>> keyframes_;
    property_timeline_map_t                  timelines_{};

    AnimationOptions options_{};

  public:
    using duration         = std::chrono::system_clock::duration;
    using frame_iterator_t = decltype(keyframes_.begin());

    explicit Animation(
      std::initializer_list<Keyframe> keyframes,
      AnimationOptions                opts = {}
    )
        : options_(opts) {
      for (const auto& kf: keyframes) {
        keyframes_.emplace_back(kf.get_position(), kf);
      }
      compile_timelines();
    }

    const property_timeline_map_t& get_timelines() const {
      return timelines_;
    }

    const AnimationOptions& get_options() const {
      return options_;
    }

  private:
    void compile_timelines() {
      timelines_.clear();
      for (auto& [x, kf]: keyframes_) {
        kf.add_to_timeline_map(timelines_);
      }
    }
  };
} // namespace cydui::animations
