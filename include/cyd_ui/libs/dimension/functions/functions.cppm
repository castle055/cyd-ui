/*! \file  functions.cppm
 *! \brief
 *!
 */

export module cydui.dimensions.functions;

import std;
import fabric.logging;
export import cydui.dimensions;


namespace cydui::dimensions::dimfn {
  template <typename T>
  typename function<T>::depset_type to_dependency_set(const std::vector<dimension<T>>& dims) {
    typename function<T>::depset_type set{};
    for (auto& dim: dims) {
      set.insert(dim.as_dependency());
    }
    return set;
  }
} // namespace cydui::dimensions::dimfn

export namespace cydui::dimensions::dimfn {
  template <typename T>
  function<T> max(std::vector<dimension<T>> dims) {
    return {
      [=] {
        auto acc = 0_px;
        for (auto d: dims) {
          acc = std::max(acc, dimensions::get_value(d));
        }
        return acc;
      },
      to_dependency_set(dims)
    };
  }

  template <typename T>
  function<T> min(std::vector<dimension<T>> dims) {
    return {
      [=] {
        screen_measure acc{std::numeric_limits<screen_measure::data_type>::max()};
        for (auto d: dims) {
          acc = std::min(acc, dimensions::get_value(d));
        }
        return acc;
      },
      to_dependency_set(dims)
    };
  }
} // namespace cydui::dimensions::dimfn
