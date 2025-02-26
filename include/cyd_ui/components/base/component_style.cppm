// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

export module cydui.components.base.style;

import std;

export import reflect;
export import quantify;

export import cydui.dimensions;
export import cydui.graphics;
export import cydui.styling;

import cydui.animations.complexity;

export namespace cyd::ui::components {
  struct style_base_t {
    [[meta(AnimationComplexity::REPAINT)]]
    vg::paint::type background{vg::paint::type::make(vg::paint::solid("#00000000"_color))};
  };

  struct style_rule_instance_t {
    std::size_t specificity;
    StyleRule::sptr rule;
    bool active{false};
    std::unordered_set<const refl::field_info*> active_base_properties{};
    std::unordered_set<const refl::field_info*> active_properties{};
  };

  class style_data_base_t {
  public:
    virtual ~style_data_base_t() = default;

    virtual void reset() = 0;
    virtual void reset_field(const refl::field_info* field, bool is_base_field) = 0;
    virtual style_base_t& as_base() = 0;
    virtual void* as_raw() = 0;
    virtual bool has_override() const = 0;
    virtual void apply_override() = 0;
    virtual void apply_transform() = 0;

    virtual void clear_style_override() = 0;
    virtual std::shared_ptr<void> get_style_override_ptr() = 0;
    virtual style_base_t& get_style_override_as_base() = 0;
    virtual void set_style_override_ptr(const std::shared_ptr<void>& other) = 0;

    std::unordered_set<std::string> tags{};
    std::vector<style_rule_instance_t> rules{};
  };

  template <typename Style>
  class style_data_t final: public style_data_base_t {
  public:
    style_data_t()
      : ptr_(std::make_shared<Style>()) {
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
      return *static_cast<Style*>(override_ptr_.get());
    }

    void set_style_override(const auto& new_style) {
      if (override_ptr_ == nullptr) {
        override_ptr_ = std::make_shared<Style>(new_style);
      } else {
        get_style_override() = new_style;
      }
    }

    void clear_style_override() override {
      override_ptr_ = nullptr;
    }

    std::shared_ptr<void> get_style_override_ptr() override {
      return override_ptr_;
    }
    style_base_t& get_style_override_as_base() override {
      return *static_cast<style_base_t*>(override_ptr_.get());
    }

    void set_style_override_ptr(const std::shared_ptr<void>& other) override {
      override_ptr_ = other;
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

    bool has_override() const override {
      return override_ptr_ != nullptr;
    }

    void apply_override() override {
      auto &s = get_style();
      const auto &so = *static_cast<Style*>(override_ptr_.get());
      s = so;
    }

    void apply_transform() override {
      style_transform(ptr_);
    }

  private:
    std::shared_ptr<void> ptr_{};
    std::shared_ptr<void> override_ptr_{nullptr};
    std::function<void(std::shared_ptr<void>&)> style_transform{[](std::shared_ptr<void>&){}};
  };
} // namespace cyd::ui::components
