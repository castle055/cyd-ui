// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

export module cydui.components.base.style;

import std;

export import reflect;
export import quantify;

export import cydui.graphics;
export import cydui.styling;

import cydui.animations.complexity;

namespace cydui {
  export struct custom_style_parser {
    explicit constexpr custom_style_parser(refl::any (*fun)(const std::string&))
        : parser_function(fun) {}

    refl::any (*parser_function)(const std::string&);
  };
}

export namespace cydui::components {
  template <typename T>
  struct four_sided_property {
  public:
    T top{}, right{}, bottom{}, left{};
  };
  template <typename T>
  struct four_cornered_property {
  public:
    T top_left{}, top_right{}, bottom_right{}, bottom_left{};
  };

  struct style_base_t {
  public:
    [[meta(AnimationComplexity::REFLOW)]] dimension_t x;
    [[meta(AnimationComplexity::REFLOW)]] dimension_t y;
    [[meta(AnimationComplexity::REFLOW)]] dimension_t width;
    [[meta(AnimationComplexity::REFLOW)]] dimension_t height;
    [[meta(AnimationComplexity::REFLOW)]] four_sided_property<dimension_t> margin;
    [[meta(AnimationComplexity::REFLOW)]] four_sided_property<dimension_t> padding;

    [[meta(AnimationComplexity::REPAINT)]]
    vg::paint::type background{vg::paint::type::make(vg::paint::solid("#00000000"_color))};

    [[meta(AnimationComplexity::REPAINT)]]
    four_sided_property<vg::paint::type> border{
      vg::paint::type::make(vg::paint::solid()),
      vg::paint::type::make(vg::paint::solid()),
      vg::paint::type::make(vg::paint::solid()),
      vg::paint::type::make(vg::paint::solid())
    };
    [[meta(AnimationComplexity::REPAINT)]] four_sided_property<int> border_width{0,0,0,0};
    [[meta(AnimationComplexity::REPAINT)]] four_cornered_property<int> border_radius{0,0,0,0};
    [[meta(AnimationComplexity::REPAINT)]] four_sided_property<std::valarray<double>> border_dasharray{};

    [[meta(AnimationComplexity::COMPOSE)]] la::scalar opacity{1.0f};
    [[meta(AnimationComplexity::COMPOSE)]] la::scalar translate_x{0.0f};
    [[meta(AnimationComplexity::COMPOSE)]] la::scalar translate_y{0.0f};
    [[meta(AnimationComplexity::COMPOSE)]] la::scalar scale_x{1.0f};
    [[meta(AnimationComplexity::COMPOSE)]] la::scalar scale_y{1.0f};

    [[meta(AnimationComplexity::COMPOSE)]]
    angle_type rotation{0.0f};
  };
  struct simple_style_t: style_base_t {};

  struct style_rule_instance_t {
    std::size_t specificity;
    StyleRule::sptr rule;
    bool active{false};
    std::unordered_set<const refl::field_info*> active_base_properties{};
    std::unordered_set<const refl::field_info*> active_properties{};
  };

  struct style_override_data_t {
    void clear() {
      base_fields.clear();
      fields.clear();
    }

    bool operator==(const style_override_data_t& other) const {
      return base_fields == other.base_fields and fields == other.fields;
    }

    std::unordered_map<refl::field_path, refl::any> base_fields{};
    std::unordered_map<refl::field_path, refl::any> fields{};
  };

  class style_data_base_t {
  public:
    virtual ~style_data_base_t() = default;

    virtual void reset() = 0;
    virtual void reset_field(const refl::field_info* field, bool is_base_field) = 0;
    virtual style_base_t& as_base() = 0;
    virtual void* as_raw() = 0;
    virtual void apply_override() = 0;
    virtual void apply_transform() = 0;

    virtual const std::shared_ptr<dimension_ctx_t>& get_dimensional_ctx() = 0;

    virtual void set_base_field_override(refl::field_path field_path, refl::any value) = 0;
    virtual void set_field_override(refl::field_path field_path, refl::any value) = 0;
    virtual void clear_style_override() = 0;

    bool update_override_with(const style_override_data_t& override_data) {
      if (style_override_data != override_data) {
        style_override_data = override_data;
        return true;
      } else {
        return false;
      }
    }

    std::shared_ptr<dimension_ctx_t> get_dimension_ctx() {
      return dimension_ctx_;
    };

    std::unordered_set<std::string> tags{};
    std::vector<style_rule_instance_t> rules{};
    style_override_data_t style_override_data{};
    std::shared_ptr<dimension_ctx_t> dimension_ctx_{std::make_shared<dimension_ctx_t>()};
  };

  template <typename Style>
  class style_data_t final: public style_data_base_t {
  public:
    style_data_t(const std::string& ctx_name)
      : ptr_(std::make_shared<Style>()) {
      dimension_ctx_->set_name(ctx_name);

      auto& base = as_base();
      base.x.set_context(dimension_ctx_, "x");
      base.y.set_context(dimension_ctx_, "y");
      base.width.set_context(dimension_ctx_, "width");
      base.height.set_context(dimension_ctx_, "height");

      base.margin.top.set_context(dimension_ctx_, "margin_top");
      base.margin.right.set_context(dimension_ctx_, "margin_right");
      base.margin.bottom.set_context(dimension_ctx_, "margin_bottom");
      base.margin.left.set_context(dimension_ctx_, "margin_left");

      base.padding.top.set_context(dimension_ctx_, "padding_top");
      base.padding.right.set_context(dimension_ctx_, "padding_right");
      base.padding.bottom.set_context(dimension_ctx_, "padding_bottom");
      base.padding.left.set_context(dimension_ctx_, "padding_left");
    }

    void reset() override {
      get_style() = {};
    }

    void reset_field(const refl::field_info* field, bool is_base_field) override {
      const refl::type_info& ti = field->type();
      if (is_base_field) {
        static style_base_t default_value{};
        ti.assign_copy_of(field->get_ptr(&default_value), field->get_ptr(&as_base()));
      } else {
        static Style default_value{};
        ti.assign_copy_of(field->get_ptr(&default_value), field->get_ptr(&get_style()));
      }
    }

    auto& get_style() {
      return *static_cast<Style*>(ptr_.get());
    }

    auto& get_style_override() {
      return style_override_data;
    }

    void set_base_field_override(refl::field_path field_info, refl::any value) override {
      style_override_data.base_fields[field_info] = value;
    }
    void set_field_override(refl::field_path field_info, refl::any value) override {
      style_override_data.fields[field_info] = value;
    }
    void clear_style_override() override {
      style_override_data.clear();
    }

    const std::shared_ptr<dimension_ctx_t>& get_dimensional_ctx() override {
      return dimension_ctx_;
    }

    void set_style_transform(auto&& transform_func) {
      style_transform = [=](std::shared_ptr<void>& ptr) {
        transform_func(*static_cast<Style*>(ptr.get()));
      };
    }
    void clear_style_transform() {
      style_transform = [](std::shared_ptr<void> &) {
      };
    }

    style_base_t& as_base() override {
      return *static_cast<style_base_t*>(ptr_.get());
    }

    void* as_raw() override {
      return &get_style();
    }

    void apply_override() override {
      auto &s = get_style();
      style_base_t& base_s = as_base();

      for (const auto & [field_info, value] : style_override_data.base_fields) {
        field_info.type().assign_copy_of(value.data(), field_info.get_ptr(&base_s));
      }
      for (const auto & [field_info, value] : style_override_data.fields) {
        field_info.type().assign_copy_of(value.data(), field_info.get_ptr(&s));
      }
    }

    void apply_transform() override {
      style_transform(ptr_);
    }

  private:
    std::shared_ptr<void> ptr_{};
    std::function<void(std::shared_ptr<void>&)> style_transform{[](std::shared_ptr<void>&){}};
  };
} // namespace cydui::components
