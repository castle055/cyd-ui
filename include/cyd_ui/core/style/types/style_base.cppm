// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include <tracy/Tracy.hpp>

export module cydui.styling.style_base;

import std;
import fabric.logging;
export import reflect;

export import quantify;

export import cydui.elements;
export import cydui.layer;
export import cydui.paint;
import cydui.animations.complexity;

export namespace cydui {
  struct CustomConversion {
    const char* type_name;
    refl::any   (*converter)(const refl::any&);

    template <typename T>
    static consteval CustomConversion from(refl::any (*converter_)(const refl::any&)) noexcept {
      return CustomConversion {refl::type_name<T>.data(), converter_};
    }

    static CustomConversion from(
      const char* type_name,
      refl::any   (*converter_)(const refl::any&)) noexcept {
      return CustomConversion {type_name, converter_};
    }
  };
} // namespace cydui

namespace cydui {
  std::unordered_map<std::string, std::unordered_map<std::string, CustomConversion>>
    runtime_custom_conversions {};
}

export namespace cydui::style {
  void set_custom_type_conversion(
    std::string from_type,
    std::string to_type,
    refl::any   (*converter_)(const refl::any&)) {
    if (not runtime_custom_conversions.contains(from_type)) {
      runtime_custom_conversions[from_type] = {};
    }
    runtime_custom_conversions[from_type][to_type] = CustomConversion::from(from_type.c_str(), converter_);
  }

  template <
    typename From,
    typename To>
  void set_custom_type_conversion(refl::any (*converter_)(const refl::any&)) {
    set_custom_type_conversion(
      std::string {refl::type_name<From>}, std::string {refl::type_name<To>}, converter_);
  }

  std::optional<CustomConversion> get_custom_type_conversion(
    std::string from_type,
    std::string to_type) {
    if (
      runtime_custom_conversions.contains(from_type)
      and runtime_custom_conversions.at(from_type).contains(to_type)) {
      return runtime_custom_conversions.at(from_type).at(to_type);
    }
    return std::nullopt;
  }

  template <
    typename From,
    typename To>
  std::optional<CustomConversion> get_custom_type_conversion() {
    return get_custom_type_conversion(
      std::string {refl::type_name<From>}, std::string {refl::type_name<To>});
  }
} // namespace cydui::style

export namespace cydui::style {
  template <typename T>
  struct four_sided_property {
    T top {}, right {}, bottom {}, left {};
  };
  template <typename T>
  struct four_cornered_property {
    T top_left {}, top_right {}, bottom_right {}, bottom_left {};
  };

  struct style_base_t {
    [[meta(animations::AnimationComplexity::REFLOW)]]
    expression_t x;
    [[meta(animations::AnimationComplexity::REFLOW)]]
    expression_t y;
    [[meta(animations::AnimationComplexity::FULL_UPDATE)]]
    position_e position {position_e::RELATIVE};
    [[meta(animations::AnimationComplexity::REFLOW)]]
    expression_t width;
    [[meta(animations::AnimationComplexity::REFLOW)]]
    expression_t height;
    [[meta(animations::AnimationComplexity::REFLOW)]]
    expression_t max_width;
    [[meta(animations::AnimationComplexity::REFLOW)]]
    expression_t max_height;
    [[meta(animations::AnimationComplexity::REFLOW)]]
    four_sided_property<expression_t> margin;
    [[meta(animations::AnimationComplexity::REFLOW)]]
    four_sided_property<expression_t> padding;
    [[meta(animations::AnimationComplexity::REFLOW)]]
    expression_t scroll_x;
    [[meta(animations::AnimationComplexity::REFLOW)]]
    expression_t scroll_y;

    [[meta(animations::AnimationComplexity::REPAINT)]] [[meta(
      CustomConversion::from<Color>([](const refl::any& clr) -> refl::any {
        return refl::any::make<Paint>(paints::Solid(clr.as<Color>()));
      }))]]
    Paint background {paints::Solid("#00000000"_color)};

    [[meta(animations::AnimationComplexity::REPAINT)]]
    four_sided_property<Paint> border {
      paints::Solid {}, paints::Solid {}, paints::Solid {}, paints::Solid {}};
    [[meta(animations::AnimationComplexity::REPAINT)]]
    four_sided_property<int> border_width {0, 0, 0, 0};
    [[meta(animations::AnimationComplexity::REPAINT)]]
    four_cornered_property<int> border_radius {0, 0, 0, 0};
    [[meta(animations::AnimationComplexity::REPAINT)]]
    four_sided_property<std::valarray<double>> border_dasharray {};
    [[meta(animations::AnimationComplexity::REPAINT)]]
    four_sided_property<double> border_dashoffset {};
    [[meta(animations::AnimationComplexity::REPAINT)]]
    four_sided_property<double> border_opacity {1, 1, 1, 1};

    [[meta(animations::AnimationComplexity::COMPOSE)]]
    la::scalar opacity {1.0f};
    [[meta(animations::AnimationComplexity::COMPOSE)]]
    la::scalar translate_x {0.0f};
    [[meta(animations::AnimationComplexity::COMPOSE)]]
    la::scalar translate_y {0.0f};
    [[meta(animations::AnimationComplexity::COMPOSE)]]
    la::scalar scale_x {1.0f};
    [[meta(animations::AnimationComplexity::COMPOSE)]]
    la::scalar scale_y {1.0f};

    [[meta(animations::AnimationComplexity::COMPOSE)]]
    angle_type rotation {0.0f};


    [[meta(animations::AnimationComplexity::FULL_UPDATE)]]
    overflow_e overflow_x {overflow_e::HIDE};
    [[meta(animations::AnimationComplexity::FULL_UPDATE)]]
    overflow_e overflow_y {overflow_e::HIDE};
  };

  struct simple_style_t: style_base_t {};

  class style_object_t {
    std::shared_ptr<style_base_t> base_ref_;
    std::shared_ptr<void>         impl_ref_;

    void (*reset_fun_)(void* ptr);

  public:
    template <typename StyleType>
    explicit style_object_t(const std::shared_ptr<StyleType>& obj)
        : base_ref_(obj),
          impl_ref_(obj),
          reset_fun_([](void* ptr) {
            static StyleType default_style {};
            StyleType&       style = *static_cast<StyleType*>(ptr);
            style                  = default_style;
          }) {}

    style_base_t& get_base() {
      return *base_ref_;
    }

    const style_base_t& get_base() const {
      return *base_ref_;
    }

    void* get_impl() {
      return impl_ref_.get();
    }

    const void* get_impl() const {
      return impl_ref_.get();
    }

    void reset() {
      reset_fun_(get_impl());
    }
  };
} // namespace cydui::style

struct __static_init {
  __static_init() {
    using namespace cydui;
    using namespace cydui::style;

    set_custom_type_conversion<Color, Paint>(
      [](const refl::any& it) { return refl::any::make<Paint>(paints::Solid(it.as<Color>())); });

    set_custom_type_conversion<Color, four_sided_property<Paint>>([](const refl::any& it) {
      return refl::any::make<four_sided_property<Paint>>(four_sided_property<Paint> {
        paints::Solid(it.as<Color>()),
        paints::Solid(it.as<Color>()),
        paints::Solid(it.as<Color>()),
        paints::Solid(it.as<Color>())});
    });

    set_custom_type_conversion<double, int>([](const refl::any& it) {
      int iti = static_cast<int>(it.as<double>());
      return refl::any::make<int>(iti);
    });

    set_custom_type_conversion<double, four_sided_property<int>>([](const refl::any& it) {
      int iti = static_cast<int>(it.as<double>());
      return refl::any::make<four_sided_property<int>>(four_sided_property<int> {
        iti,
        iti,
        iti,
        iti,
      });
    });

    set_custom_type_conversion<double, four_cornered_property<int>>([](const refl::any& it) {
      int iti = static_cast<int>(it.as<double>());
      return refl::any::make<four_cornered_property<int>>(four_cornered_property<int> {
        iti,
        iti,
        iti,
        iti,
      });
    });

    set_custom_type_conversion<double, four_sided_property<double>>([](const refl::any& it) {
      double iti = it.as<double>();
      return refl::any::make<four_sided_property<double>>(four_sided_property<double> {
        iti,
        iti,
        iti,
        iti,
      });
    });

    set_custom_type_conversion<double, four_cornered_property<double>>([](const refl::any& it) {
      double iti = it.as<double>();
      return refl::any::make<four_cornered_property<double>>(four_cornered_property<double> {
        iti,
        iti,
        iti,
        iti,
      });
    });

    set_custom_type_conversion<dimensions::screen_measure, expression_t>([](const refl::any& it) {
      auto& iti = it.as<dimensions::screen_measure>();
      return refl::any::make<expression_t>(iti);
    });

    set_custom_type_conversion<dimensions::screen_measure, four_sided_property<expression_t>>(
      [](const refl::any& it) {
        auto& iti = it.as<dimensions::screen_measure>();
        return refl::any::make<four_sided_property<expression_t>>(
          four_sided_property<expression_t> {
            iti,
            iti,
            iti,
            iti,
          });
      });

    set_custom_type_conversion<dimensions::screen_measure, four_cornered_property<expression_t>>(
      [](const refl::any& it) {
        auto& iti = it.as<dimensions::screen_measure>();
        return refl::any::make<four_cornered_property<expression_t>>(
          four_cornered_property<expression_t> {
            iti,
            iti,
            iti,
            iti,
          });
      });
  }
} __static_init {};
