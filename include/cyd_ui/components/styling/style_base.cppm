// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include <tracy/Tracy.hpp>

export module cydui.styling.style_base;

import std;
export import reflect;

export import quantify;

export import cydui.graphics.vector;
export import cydui.graphics.compositing_operation;
import cydui.animations.complexity;

export namespace cydui {
  struct CustomConversion {
    refl::type_id_t type_id;
    refl::any       (*converter)(const refl::any&);

    template <typename T>
    static consteval CustomConversion from(refl::any (*converter_)(const refl::any&)) noexcept {
      return CustomConversion{refl::type_id<T>, converter_};
    }
  };
} // namespace cydui

export namespace cydui::components {
  template <typename T>
  struct four_sided_property {
    T top{}, right{}, bottom{}, left{};
  };
  template <typename T>
  struct four_cornered_property {
    T top_left{}, top_right{}, bottom_right{}, bottom_left{};
  };

  struct style_base_t {
    [[meta(AnimationComplexity::REFLOW)]] expression_t    x;
    [[meta(AnimationComplexity::REFLOW)]] expression_t    y;
    [[meta(AnimationComplexity::FULL_UPDATE)]] position_e position_x{position_e::RELATIVE};
    [[meta(AnimationComplexity::FULL_UPDATE)]] position_e position_y{position_e::RELATIVE};
    [[meta(AnimationComplexity::REFLOW)]] expression_t    width;
    [[meta(AnimationComplexity::REFLOW)]] expression_t    height;
    [[meta(AnimationComplexity::REFLOW)]] four_sided_property<expression_t> margin;
    [[meta(AnimationComplexity::REFLOW)]] four_sided_property<expression_t> padding;
    [[meta(AnimationComplexity::REFLOW)]] expression_t                      scroll_x;
    [[meta(AnimationComplexity::REFLOW)]] expression_t                      scroll_y;

    [[meta(AnimationComplexity::REPAINT)]] [[meta(
      CustomConversion::from<color::Color>([](const refl::any& clr) -> refl::any {
        return refl::any::make<vg::paint::type>(
          vg::paint::type::make(vg::paint::solid(clr.as<color::Color>()))
        );
      })
    )]]
    vg::paint::type background{vg::paint::type::make(vg::paint::solid("#00000000"_color))};

    [[meta(AnimationComplexity::REPAINT)]]
    four_sided_property<vg::paint::type> border{
      vg::paint::type::make(vg::paint::solid()),
      vg::paint::type::make(vg::paint::solid()),
      vg::paint::type::make(vg::paint::solid()),
      vg::paint::type::make(vg::paint::solid())
    };
    [[meta(AnimationComplexity::REPAINT)]] four_sided_property<int>    border_width{0, 0, 0, 0};
    [[meta(AnimationComplexity::REPAINT)]] four_cornered_property<int> border_radius{0, 0, 0, 0};
    [[meta(
      AnimationComplexity::REPAINT
    )]] four_sided_property<std::valarray<double>>                     border_dasharray{};

    [[meta(AnimationComplexity::COMPOSE)]] la::scalar opacity{1.0f};
    [[meta(AnimationComplexity::COMPOSE)]] la::scalar translate_x{0.0f};
    [[meta(AnimationComplexity::COMPOSE)]] la::scalar translate_y{0.0f};
    [[meta(AnimationComplexity::COMPOSE)]] la::scalar scale_x{1.0f};
    [[meta(AnimationComplexity::COMPOSE)]] la::scalar scale_y{1.0f};

    [[meta(AnimationComplexity::COMPOSE)]]
    angle_type rotation{0.0f};


    [[meta(AnimationComplexity::FULL_UPDATE)]]
    overflow_e overflow_x{overflow_e::HIDE};
    [[meta(AnimationComplexity::FULL_UPDATE)]]
    overflow_e overflow_y{overflow_e::HIDE};
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
            static StyleType default_style{};
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
} // namespace cydui::components
