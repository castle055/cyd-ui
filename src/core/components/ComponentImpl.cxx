// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module cydui.core.Component.impl;
import :parent_reference;

import std;

import fabric.logging;
import fabric.async;
import fabric.templates.functor_arguments;

import cydui.core.blueprint.base;
import cydui.core.aspects.contexts.store;

import cydui.geometry;
import cydui.styling.style_stack;
import cydui.core.ui.services.AnimationService;


using namespace cydui;
using namespace cydui::detail;
using ui::services::AnimationService;

ComponentImpl::ComponentImpl(
  fabric::async::async_bus_t&       bus,
  fabric::services::ServiceContext& internal_services,
  fabric::services::ServiceContext& ui_services,
  parent_reference_t                parent,
  Blueprint::uptr                   blueprint,
  const ComponentState::sptr&       state)
    : internal_services_(internal_services),
      ui_services_(ui_services),
      blueprint_(std::move(blueprint)),
      id_(blueprint_->get_id()),
      parent_(parent),
      children_ {},
      state_(state == nullptr ? blueprint_->make_state_object() : state),
      style_stack_(
        blueprint_->get_style_type_info(),
        blueprint_->make_style_object()),
      geometry_(layout::component_geometry::make(get_name())),
      context_store_(parent_.is_valid() ? &parent_->context_store_ : nullptr),
      event_dispatcher_(blueprint_->make_event_dispatcher(
        bus,
        ui_services,
        parent_.is_valid() ? parent_->event_dispatcher_.get() : nullptr,
        this,
        *state_,
        context_store_)) {}

ComponentImpl* ComponentImpl::make(
  fabric::async::async_bus_t& bus,
  ComponentImpl&              parent,
  Blueprint::uptr             blueprint,
  const ComponentState::sptr& state) {
  auto ptr = std::unique_ptr<ComponentImpl>(new ComponentImpl {
    bus, parent.internal_services_, parent.ui_services_, parent_reference_t {&parent}, std::move(blueprint), state});

  auto* ret = ptr.get();
  parent.children_.emplace_back(std::move(ptr));

  return ret;
}

ComponentImpl::uptr ComponentImpl::make_root(
  fabric::async::async_bus_t&       bus,
  fabric::services::ServiceContext& internal_services,
  fabric::services::ServiceContext& ui_services,
  Blueprint::uptr                   blueprint) {
  return std::unique_ptr<ComponentImpl>(new ComponentImpl {
    bus, internal_services, ui_services, parent_reference_t {nullptr}, std::move(blueprint), nullptr});
}

ComponentImpl::~ComponentImpl() {
  event_dispatcher_->dispatch_dismount();
}

void ComponentImpl::mark_dirty() {
  __dirty__ = true;
}

bool ComponentImpl::is_dirty() const {
  return __dirty__;
}

void ComponentImpl::clear_dirty_flag() {
  __dirty__ = false;
}

bool ComponentImpl::is_hovered() const {
  return hovering_;
}

void ComponentImpl::set_hovered(bool hovered) {
  hovering_ = hovered;
}

bool ComponentImpl::is_focused() const {
  return focused_;
}

void ComponentImpl::focus() {
  if (not focused_) {
    focused_ = true;
    event_dispatcher_->dispatch_focus_changed();
  }
}

void ComponentImpl::unfocus() {
  if (focused_) {
    focused_ = false;
    event_dispatcher_->dispatch_focus_changed();
  }
}

bool ComponentImpl::is_text_input() const {
  return is_text_input_;
}

bool ComponentImpl::is_animated() const {
  return not animations_.empty();
}

animations::AnimationHandle ComponentImpl::start_animation(animations::AnimationHandle handle) {
  animations_.push_back(handle);
  const auto it = std::prev(animations_.end());
  return animations::AnimationHandle {[=, this]() {
    it->stop();
    animations_.erase(it);
  }};
}

void ComponentImpl::stop_all_animation() {
  for (auto& animation: animations_) {
    animation.stop();
  }
  animations_.clear();
}

void ComponentImpl::mount() {
  get_event_dispatcher().dispatch_mount(blueprint_->get_content());
  Component* self = this;
  blueprint_->update_references(self);
}

detail::update_result ComponentImpl::update_with(const Blueprint& other) {
  auto       res  = blueprint_->update_with(other);
  Component* self = this;
  blueprint_->update_references(self);
  return res;
}

std::optional<Component*> ComponentImpl::find_child(const std::string& id) {
  for (auto& child: children_) {
    if (child->get_id().str() == id) {
      return child.get();
    }
  }
  return std::nullopt;
}

std::optional<const Component*> ComponentImpl::find_child(const std::string& id) const {
  for (const auto& child: children_) {
    if (child->get_id().str() == id) {
      return child.get();
    }
  }
  return std::nullopt;
}

std::list<Component*> ComponentImpl::find_children(const std::string& tag) {
  std::list<Component*> result {};
  for (auto& child: children_) {
    if (child->get_blueprint().has_tag(tag)) {
      result.emplace_back(child.get());
    }
  }
  return result;
}

std::list<const Component*> ComponentImpl::find_children(const std::string& tag) const {
  std::list<const Component*> result {};
  for (const auto& child: children_) {
    if (child->get_id().str() == tag) {
      result.emplace_back(child.get());
    }
  }
  return result;
}

std::list<Component*> ComponentImpl::find_descendents(
  const std::string& tag,
  bool               skip_direct_children) {
  std::list<Component*> result {};
  if (not skip_direct_children) {
    for (auto& child: children_) {
      if (child->get_blueprint().has_tag(tag)) {
        result.emplace_back(child.get());
      }
    }
  }
  for (auto& child: children_) {
    auto descendants = child->find_descendents(tag, false);
    for (auto& d: descendants) {
      result.emplace_back(d);
    }
  }
  return result;
}

std::list<const Component*> ComponentImpl::find_descendents(
  const std::string& tag,
  bool               skip_direct_children) const {
  std::list<const Component*> result {};
  if (not skip_direct_children) {
    for (const auto& child: children_) {
      if (child->get_blueprint().has_tag(tag)) {
        result.emplace_back(child.get());
      }
    }
  }
  for (const auto& child: children_) {
    auto descendants = child->find_descendents(tag, false);
    for (const auto& d: descendants) {
      result.emplace_back(d);
    }
  }
  return result;
}

std::optional<ComponentImpl*> ComponentImpl::find_child_impl(const std::string& id) {
  for (auto& child: children_) {
    if (child->get_id().str() == id) {
      return child.get();
    }
  }
  return std::nullopt;
}

std::optional<const ComponentImpl*> ComponentImpl::find_child_impl(const std::string& id) const {
  for (const auto& child: children_) {
    if (child->get_id().str() == id) {
      return child.get();
    }
  }
  return std::nullopt;
}

std::list<ComponentImpl*> ComponentImpl::find_children_impl(const std::string& tag) {
  std::list<ComponentImpl*> result {};
  for (auto& child: children_) {
    if (child->get_blueprint().has_tag(tag)) {
      result.emplace_back(child.get());
    }
  }
  return result;
}

std::list<const ComponentImpl*> ComponentImpl::find_children_impl(const std::string& tag) const {
  std::list<const ComponentImpl*> result {};
  for (const auto& child: children_) {
    if (child->get_id().str() == tag) {
      result.emplace_back(child.get());
    }
  }
  return result;
}

std::list<ComponentImpl*> ComponentImpl::find_descendents_impl(
  const std::string& tag,
  bool               skip_direct_children) {
  std::list<ComponentImpl*> result {};
  if (not skip_direct_children) {
    for (auto& child: children_) {
      if (child->get_blueprint().has_tag(tag)) {
        result.emplace_back(child.get());
      }
    }
  }
  for (auto& child: children_) {
    auto descendants = child->find_descendents_impl(tag, false);
    for (auto& d: descendants) {
      result.emplace_back(d);
    }
  }
  return result;
}

std::list<const ComponentImpl*> ComponentImpl::find_descendents_impl(
  const std::string& tag,
  bool               skip_direct_children) const {
  std::list<const ComponentImpl*> result {};
  if (not skip_direct_children) {
    for (const auto& child: children_) {
      if (child->get_blueprint().has_tag(tag)) {
        result.emplace_back(child.get());
      }
    }
  }
  for (const auto& child: children_) {
    auto descendants = child->find_descendents_impl(tag, false);
    for (const auto& d: descendants) {
      result.emplace_back(d);
    }
  }
  return result;
}

bool ComponentImpl::is_root() const {
  return not parent_.is_valid();
}

const ComponentIdentifier& ComponentImpl::get_id() const {
  return id_;
}

const std::string& ComponentImpl::get_name() const {
  return blueprint_->get_name();
}

ComponentState::sptr ComponentImpl::get_state() const {
  return state_;
}

Blueprint& ComponentImpl::get_blueprint() {
  return *blueprint_.get();
}

const Blueprint& ComponentImpl::get_blueprint() const {
  return *blueprint_.get();
}

std::list<Component*> ComponentImpl::get_children() {
  std::list<Component*> res {};
  for (const auto& component_impl: children_) {
    res.emplace_back(component_impl.get());
  }
  return res;
}

std::list<const Component*> ComponentImpl::get_children() const {
  std::list<const Component*> res {};
  for (const auto& component_impl: children_) {
    res.emplace_back(component_impl.get());
  }
  return res;
}

std::list<ComponentImpl::uptr>& ComponentImpl::get_children_impl() {

  return children_;
}

const std::list<ComponentImpl::uptr>& ComponentImpl::get_children_impl() const {
  return children_;
}

Component* ComponentImpl::get_parent() {
  return &*parent_;
}

const Component* ComponentImpl::get_parent() const {
  return &*parent_;
}

const parent_reference_t& ComponentImpl::get_parent_impl() const {
  return parent_;
}

event_dispatcher_base_t& ComponentImpl::get_event_dispatcher() const {
  return *event_dispatcher_;
}

layout::component_geometry& ComponentImpl::get_geometry() {
  return *geometry_;
}

layout::component_geometry& ComponentImpl::get_geometry() const {
  return *geometry_;
}

Layer& ComponentImpl::get_layer() {
  return render_layer_;
}

const Layer& ComponentImpl::get_layer() const {
  return render_layer_;
}

const style::style_base_t& ComponentImpl::get_style() const {
  return style_stack_.get_style_object();
}

style::style_stack& ComponentImpl::get_style_stack() {
  return style_stack_;
}

animations::AnimationHandle ComponentImpl::animate(const animations::Animation& anim) {
  auto anim_service_opt = internal_services_.find<AnimationService>();
  if (not anim_service_opt.has_value()) {
    throw fabric::exception("Animation service not found");
  }
  auto& anim_service = *anim_service_opt.value();
  anim_service.enable();
  auto handle = anim_service.start_animation(anim, *this);
  return handle;
}
