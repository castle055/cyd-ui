// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#define STYLE_SETTER_RETURN_TYPE Blueprint&
#define STYLE_SETTER_RETURN_EXPR return *this;
#define STYLE_SETTER_REF_CONSTRAINT
#define STYLE_MAP_GETTER this->style_map_
#include "../../../style/include/style_setters_detail.h"

export module cydui.core.blueprint.base;
export import :content;
export import :event_dispatcher;

import std;
export import reflect;
import fabric.logging;
import fabric.exception;
import fabric.services;

export import cydui.core.identifier;
export import cydui.core.state;
export import cydui.styling.sparse_style_map;
export import cydui.core.aspects.contexts.store;
export import cydui.core.aspects.reference.iface;


export namespace cydui::detail {
  using tag_type    = std::string;
  using tagset_type = std::unordered_set<tag_type>;

  struct update_result {
    bool needs_update {false};
    bool needs_restyle {false};
  };
} // namespace cydui::detail

export namespace cydui {
  class Blueprint {
    ComponentIdentifier id_ {};
    std::string         name_ {};

  protected:
    detail::tagset_type     tags_ {};
    style::sparse_style_map style_map_;
    BlueprintList           content_ {};

    detail::reference_set references_ {};

  public:
    using uptr = std::unique_ptr<Blueprint>;

    Blueprint(
      ComponentIdentifier    id,
      std::string            name,
      const refl::type_info& style_ti)
        : id_(id),
          name_(name),
          style_map_(style_ti) {}

    virtual ~Blueprint() = default;

    Blueprint(const Blueprint& other)
        : id_(other.id_),
          name_(other.name_),
          tags_(other.tags_),
          style_map_(other.style_map_),
          content_(other.content_),
          references_(other.references_) {}

  public:
    virtual detail::ComponentState::sptr make_state_object() const = 0;

    virtual std::unique_ptr<detail::event_dispatcher_base_t> make_event_dispatcher(
      fabric::async::async_bus_t&       bus,
      fabric::services::ServiceContext& service_context,
      detail::event_dispatcher_base_t*  parent,
      void*                             component,
      detail::ComponentState&           state,
      detail::context_store_t&          context_store) const = 0;

    virtual style::style_object_t make_style_object() const = 0;

    virtual detail::update_result update_with(const Blueprint& other) = 0;

    virtual const refl::type_info& get_style_type_info() const = 0;

    virtual refl::any_ref get_props() = 0;

    virtual bool handles_text_input() const = 0;

    virtual uptr clone() const = 0;

    void update_references(void* mounted_component) {
      for (auto& ref: references_) {
        ref->set_reference(mounted_component);
      }
    }

  public:
    const ComponentIdentifier& get_id() const {
      return id_;
    }
    void set_id(const std::string& id) {
      id_.set_id(id);
    }

    const std::string& get_name() const {
      return name_;
    }

    const BlueprintList& get_content() const {
      return content_;
    }

    const style::sparse_style_map& get_style_override() const {
      return style_map_;
    }

    template <typename T>
    bool is_type() const {
      return dynamic_cast<const T*>(this) != nullptr;
    }

    template <typename T>
    const T& as() const {
      const T* ptr = dynamic_cast<const T*>(this);
      if (ptr == nullptr) {
        throw fabric::exception {std::format("Bad cast: expected '{}', found '{}'", refl::type_name<T>, get_name())};
      }
      return *ptr;
    }

    template <typename T>
    T& as() {
      T* ptr = dynamic_cast<T*>(this);
      if (ptr == nullptr) {
        throw fabric::exception {std::format("Bad cast: expected '{}', found '{}'", refl::type_name<T>, get_name())};
      }
      return *ptr;
    }

  public:
    void tag(const detail::tagset_type& tags) {
      for (const auto& tag: tags) {
        tags_.insert(tag);
      }
    }

    void tag(const detail::tag_type& tag) {
      tags_.insert(tag);
    }

    void tag(
      const detail::tag_type& tag,
      bool                    tagged) {
      if (tagged) {
        this->tag(tag);
      } else {
        untag(tag);
      }
    }

    void untag(const detail::tagset_type& tags) {
      for (const auto& tag: tags) {
        tags_.erase(tag);
      }
    }

    void untag(const detail::tag_type& tag) {
      tags_.erase(tag);
    }

    bool has_tag(const detail::tag_type& tag) const {
      return tags_.contains(tag);
    }

    detail::tagset_type& get_tags() {
      return tags_;
    }

    const detail::tagset_type& get_tags() const {
      return tags_;
    }

    Blueprint& operator()(BlueprintList&& _content_) {
      this->content_ = _content_;
      return *this;
    }

    Blueprint& operator()(const BlueprintList& _content_) {
      this->content_ = _content_;
      return *this;
    }

  public:
#include "../../../style/include/style_setters.inc"
  };

  BlueprintList::~BlueprintList() = default;

  BlueprintList::BlueprintList(const BlueprintList& other) {
    for (const auto& blueprint: other) {
      this->emplace_back(std::move(blueprint->clone()));
    }
  }

  BlueprintList& BlueprintList::operator=(const BlueprintList& other) {
    this->clear();
    for (const auto& blueprint: other) {
      this->emplace_back(std::move(blueprint->clone()));
    }
    return (*this);
  }

  void BlueprintList::add_element(const BlueprintList& arg) {
    for (const auto& blueprint: arg) {
      this->emplace_back(blueprint->clone());
    }
  }

  void BlueprintList::add_element(const Blueprint& arg) {
    this->emplace_back(arg.clone());
  }

  template <typename T>
  concept ComponentBlueprint = std::derived_from<T, Blueprint>;


  // template <typename T>
  // using match = fabric::match<T, BlueprintList>;

  auto match(const auto& value) {
    return fabric::match {value, BlueprintList {}};
  }

  auto when(const bool& condition) {
    return match(condition).if_true();
  }

  auto unless(const bool& condition) {
    return match(not condition).if_true();
  }
} // namespace cydui
