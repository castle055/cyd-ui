// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include <tracy/Tracy.hpp>

export module cydui.components.base.style;

import std;

export import reflect;
export import quantify;

export import cydui.graphics.vector;
export import cydui.graphics.compositing_operation;
export import cydui.styling.lang;
export import cydui.styling.style_base;
export import cydui.styling.style_override;

import cydui.animations.complexity;

export namespace cydui::components {
  class component_style_base {
  public:
    std::vector<StyleRuleInstance> rules{};

    virtual ~component_style_base() = default;

    virtual void reset() = 0;
    virtual void reset_field(
      const refl::field_info* field,
      bool                    is_base_field
    )                                                                          = 0;
    virtual style_base_t& as_base()                                            = 0;
    virtual void*         as_ptr()                                             = 0;
    virtual void          apply_override(const style_override& style_override) = 0;
  };

  template <typename Style>
  class component_style_impl final: public component_style_base {
    std::shared_ptr<void> ptr_{};

  public:
    explicit component_style_impl(const std::string& ctx_name)
        : ptr_(std::make_shared<Style>()) {}

    style_base_t& as_base() override {
      return *static_cast<style_base_t*>(ptr_.get());
    }

    void* as_ptr() override {
      return &get_style();
    }

    void reset() override {
      get_style() = {};
    }

    void reset_field(
      const refl::field_info* field,
      bool                    is_base_field
    ) override {
      static Style           default_value{};
      const refl::type_info& ti = field->type();
      if (is_base_field) {
        ti.assign_copy_of(
          field->get_ptr(static_cast<style_base_t*>(&default_value)), field->get_ptr(&as_base())
        );
      } else {
        ti.assign_copy_of(field->get_ptr(&default_value), field->get_ptr(&get_style()));
      }
    }

    auto& get_style() {
      return *static_cast<Style*>(ptr_.get());
    }

    void apply_override(const style_override& style_override) override {
      ZoneScopedN("Apply Override");
      auto&         s                   = get_style();
      style_base_t& base_s              = as_base();
      const auto&   style_override_data = style_override.get_style_override();

      for (const auto& [field_info, value]: style_override_data.base_fields) {
        ZoneScopedN("Base Field");
        field_info.type().assign_copy_of(value.data(), field_info.get_ptr(&base_s));
      }
      for (const auto& [field_info, value]: style_override_data.fields) {
        ZoneScopedN("Field");
        field_info.type().assign_copy_of(value.data(), field_info.get_ptr(&s));
      }
    }
  };
} // namespace cydui::components
