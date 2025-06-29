// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#define STYLE_SETTER_RETURN_TYPE void
#define STYLE_SETTER_REF_CONSTRAINT
#define STYLE_SETTER_RETURN_EXPR
#define STYLE_MAP_GETTER this->style_map_
#include "../../styling/style_setters_detail.h"

export module cydui.components.blueprint.base;
export import :event_dispatcher;

import std;
export import reflect;
import fabric.logging;

export import cydui.components.identifier;
export import cydui.components.state;
export import cydui.graphics.vector;
export import cydui.graphics.compositing_operation;
export import cydui.styling.lang;
export import cydui.styling.style_base;
export import cydui.styling.style_override;


export namespace cydui::components {
  using tag_type    = std::string;
  using tagset_type = std::unordered_set<tag_type>;

  class content_type: public std::vector<content_element> {
  public:
    template <typename... Args>
    content_type(Args&&... args) {
      (add_element(std::forward<Args>(args)), ...);
    }

    ~content_type();

    content_type(const content_type& other);
    content_type& operator=(const content_type& other);

  private:
    template <typename Arg>
    void add_element(Arg&& arg) {
      if constexpr (std::is_same_v<Arg, content_type>) {
        for (const auto& blueprint: arg) {
          this->emplace_back(blueprint->clone());
        }
      } else {
        this->emplace_back(arg.clone());
      }
    }
  };

  class blueprint_base_t {
    identifier_t id_{};
    std::string  name_{};
    tagset_type  tags_{};

  protected:
    sparse_style_map style_map_;
    content_type     content_{};

  public:
    using uptr = std::unique_ptr<blueprint_base_t>;

    blueprint_base_t(
      identifier_t           id,
      std::string            name,
      const refl::type_info& style_ti
    )
        : id_(id),
          name_(name),
          style_map_(style_ti) {}

    virtual ~blueprint_base_t() = default;

    blueprint_base_t(const blueprint_base_t& other)
        : id_(other.id_),
          name_(other.name_),
          tags_(other.tags_),
          style_map_(other.style_map_),
          content_(other.content_) {}

  public:
    virtual component_state_t::sptr make_state_object() const = 0;

    virtual std::unique_ptr<event_dispatcher_base_t>
    make_event_dispatcher(void* component) const = 0;

    virtual style_object_t make_style_object() const = 0;

    virtual bool update_with(const blueprint_base_t& other) = 0;

    virtual const refl::type_info& get_style_type_info() const = 0;

    virtual refl::any_ref get_props() = 0;

    virtual bool handles_text_input() const = 0;

    virtual uptr clone() const = 0;

  public:
    const identifier_t& get_id() const {
      return id_;
    }
    void set_id(const std::string& id) {
      id_.set_id(id);
    }

    const std::string& get_name() const {
      return name_;
    }

    const content_type& get_content() const {
      return content_;
    }

    const sparse_style_map& get_style_override() const {
      return style_map_;
    }

  public:
    void tag(const tagset_type& tags) {
      for (const auto& tag: tags) {
        tags_.insert(tag);
      }
    }

    void tag(const tag_type& tag) {
      tags_.insert(tag);
    }

    void untag(const tagset_type& tags) {
      for (const auto& tag: tags) {
        tags_.erase(tag);
      }
    }

    void untag(const tag_type& tag) {
      tags_.erase(tag);
    }

    bool has_tag(const tag_type& tag) const {
      return tags_.contains(tag);
    }

    tagset_type& get_tags() {
      return tags_;
    }

    const tagset_type& get_tags() const {
      return tags_;
    }

  public:
#include "../../styling/style_setters.inc"
  };

  content_type::~content_type() = default;

  content_type::content_type(const content_type& other) {
    for (const auto& blueprint: other) {
      this->emplace_back(std::move(blueprint->clone()));
    }
  }

  content_type& content_type::operator=(const content_type& other) {
    this->clear();
    for (const auto& blueprint: other) {
      this->emplace_back(std::move(blueprint->clone()));
    }
    return (*this);
  }

  template <typename T>
  concept ComponentBlueprint = std::derived_from<T, blueprint_base_t>;
} // namespace cydui::components
