//
// Created by castle on 2/18/25.
//
export module cydui.animations.PropertyTimeline;

import std;
import reflect;

import fabric.logging;
export import cydui.easing_functions;

export namespace cydui::animations {
  using property_id_t = std::pair<refl::type_id_t, std::string>;
}

export template <>
struct std::hash<cydui::animations::property_id_t> {
  std::size_t operator()(const cydui::animations::property_id_t& x) const noexcept {
    return std::hash<std::size_t>{}(x.first) ^ (std::hash<std::string>{}(x.second) << 1);
  }
};

export namespace cydui::animations {
  struct property_timeline_t {
    refl::any interpolate(
      double    x,
      refl::any initial_value
    ) const {
      if (keyframes.empty()) {
        return initial_value;
      }

      if (x >= std::get<0>(keyframes.back())) {
        return std::get<1>(keyframes.back());
      }

      const refl::any* prev   = &initial_value;
      double           prev_x = 0.0;
      for (const auto& [x_f, kf, interpolator, easing_fun]: keyframes) {
        if (x > x_f) {
          prev   = &kf;
          prev_x = x_f;
        } else {
          float z = (x - prev_x) / (x_f - prev_x);
          return interpolator(*prev, kf, easing_fun(z));
        }
      }

      return initial_value;
    }

    std::list<std::tuple<
      double,
      refl::any,
      std::function<refl::any(refl::any, refl::any, double)>,
      easing::function_type>>
      keyframes{};
  };

  using property_timeline_map_t = std::unordered_map<property_id_t, property_timeline_t>;
} // namespace cydui::animations
