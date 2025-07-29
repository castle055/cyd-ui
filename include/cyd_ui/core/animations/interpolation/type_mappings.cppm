//
// Created by castle on 2/22/25.
//

export module cydui.interpolation:mappings;

import std;

export namespace cydui::animations {
  template <typename T>
  struct interp_mapping;

  template <typename T>
  concept HasInterpolationMapping = requires(T t) {
    { interp_mapping<T>::to_float(t) } -> std::same_as<float>;
    { interp_mapping<T>::from_float(std::declval<float>()) } -> std::same_as<T>;
  };

  template <typename T>
  struct static_cast_interp_mapping {
    static float to_float(const T& val) {
      return static_cast<float>(val);
    }

    static T from_float(float val) {
      return static_cast<T>(val);
    }
  };
} // namespace cydui::animations

export template <>
struct cydui::animations::interp_mapping<int>: static_cast_interp_mapping<int> {};

export template <>
struct cydui::animations::interp_mapping<long>: static_cast_interp_mapping<long> {};

export template <>
struct cydui::animations::interp_mapping<float>: static_cast_interp_mapping<float> {};

export template <>
struct cydui::animations::interp_mapping<double>: static_cast_interp_mapping<double> {};
