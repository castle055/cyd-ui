// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module cydui.core.mounted;
import :parent_reference;

import std;

import fabric.logging;
import fabric.async;
import fabric.templates.functor_arguments;

import cydui.core.blueprint.base;
import cydui.core.contexts.store;

import cydui.graphics.compositing_node;

import cydui.geometry;
import cydui.styling.style_stack;


using namespace cydui;
using namespace cydui::core;

mounted_component_t::mounted_component_t(
  const backends::frame_base::sptr& window,
  parent_reference_t                parent,
  blueprint_base_t::uptr            blueprint,
  const component_state_t::sptr&    state
)
    : blueprint_(std::move(blueprint)),
      id_(blueprint_->get_id()),
      parent_(parent),
      children_{},
      window_(window),
      state_(state == nullptr ? blueprint_->make_state_object() : state),
      style_stack_(
        blueprint_->get_style_type_info(),
        blueprint_->make_style_object()
      ),
      geometry_(layout::component_geometry::make(get_name())),
      compositing_node_(
        std::make_shared<compositing::compositing_node_t>(
          parent_.is_valid() ? parent_->compositing_node_ : nullptr
        )
      ),
      context_store_({}),
      event_dispatcher_(blueprint_->make_event_dispatcher(this)) {}

mounted_component_t* mounted_component_t::make(
  mounted_component_t&    parent,
  blueprint_base_t::uptr  blueprint,
  component_state_t::sptr state
) {
  auto ptr = std::unique_ptr<mounted_component_t>(new mounted_component_t{
    parent.window_, parent_reference_t{&parent}, std::move(blueprint), state
  });

  auto* ret = ptr.get();
  parent.children_.emplace_back(std::move(ptr));

  return ret;
}

mounted_component_t::uptr mounted_component_t::make(
  const backends::frame_base::sptr& window,
  blueprint_base_t::uptr            blueprint
) {
  return std::unique_ptr<mounted_component_t>(
    new mounted_component_t{window, parent_reference_t{nullptr}, std::move(blueprint), nullptr}
  );
}

mounted_component_t::~mounted_component_t() {
  event_dispatcher_->dispatch_dismount();
}

void mounted_component_t::mark_dirty() {
  __dirty__ = true;
}

void mounted_component_t::force_update() {
  mark_dirty();
  window_->emit<RedrawEvent>({.component = this});
}

bool mounted_component_t::is_dirty() const {
  return __dirty__;
}

void mounted_component_t::clear_dirty_flag() {
  __dirty__ = false;
}

bool mounted_component_t::is_hovered() const {
  return hovering_;
}

void mounted_component_t::set_hovered(bool hovered) {
  hovering_ = hovered;
}

bool mounted_component_t::is_focused() const {
  return focused_;
}

void mounted_component_t::focus() {
  if (not focused_) {
    focused_ = true;
    event_dispatcher_->dispatch_focus_changed();
  }
}

void mounted_component_t::unfocus() {
  if (focused_) {
    focused_ = false;
    event_dispatcher_->dispatch_focus_changed();
  }
}

bool mounted_component_t::is_text_input() const {
  return is_text_input_;
}

bool mounted_component_t::is_animated() const {
  return animation_count_ > 0;
}

void mounted_component_t::start_animation() {
  animation_count_++;
}

void mounted_component_t::stop_animation() {
  animation_count_--;
}

bool mounted_component_t::update_with(const blueprint_base_t& other) {
  return blueprint_->update_with(other);
}

std::optional<mounted_component_t*> mounted_component_t::find_child(const std::string& id) {
  for (auto& child: children_) {
    if (child->get_id().str() == id) {
      return child.get();
    }
  }
  return std::nullopt;
}

std::optional<const mounted_component_t*>
mounted_component_t::find_child(const std::string& id) const {
  for (const auto& child: children_) {
    if (child->get_id().str() == id) {
      return child.get();
    }
  }
  return std::nullopt;
}

std::list<mounted_component_t*> mounted_component_t::find_children(const std::string& tag) {
  std::list<mounted_component_t*> result{};
  for (auto& child: children_) {
    if (child->get_blueprint()->has_tag(tag)) {
      result.emplace_back(child.get());
    }
  }
  return result;
}

std::list<const mounted_component_t*>
mounted_component_t::find_children(const std::string& tag) const {
  std::list<const mounted_component_t*> result{};
  for (const auto& child: children_) {
    if (child->get_id().str() == tag) {
      result.emplace_back(child.get());
    }
  }
  return result;
}

std::list<mounted_component_t*> mounted_component_t::find_descendents(
  const std::string& tag,
  bool               skip_direct_children
) {
  std::list<mounted_component_t*> result{};
  if (not skip_direct_children) {
    for (auto& child: children_) {
      if (child->get_blueprint()->has_tag(tag)) {
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

std::list<const mounted_component_t*> mounted_component_t::find_descendents(
  const std::string& tag,
  bool               skip_direct_children
) const {
  std::list<const mounted_component_t*> result{};
  if (not skip_direct_children) {
    for (const auto& child: children_) {
      if (child->get_blueprint()->has_tag(tag)) {
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

bool mounted_component_t::is_root() const {
  return not parent_.is_valid();
}

const identifier_t& mounted_component_t::get_id() const {
  return id_;
}

const std::string& mounted_component_t::get_name() const {
  return blueprint_->get_name();
}

component_state_t::sptr mounted_component_t::get_state() const {
  return state_;
}

blueprint_base_t* mounted_component_t::get_blueprint() {
  return blueprint_.get();
}

const blueprint_base_t* mounted_component_t::get_blueprint() const {
  return blueprint_.get();
}

std::list<mounted_component_t::uptr>& mounted_component_t::get_children() {

  return children_;
}

const std::list<mounted_component_t::uptr>& mounted_component_t::get_children() const {
  return children_;
}

const parent_reference_t& mounted_component_t::get_parent() const {
  return parent_;
}

event_dispatcher_base_t& mounted_component_t::get_event_dispatcher() const {
  return *event_dispatcher_;
}

layout::component_geometry& mounted_component_t::get_geometry() {
  return *geometry_;
}

const layout::component_geometry& mounted_component_t::get_geometry() const {
  return *geometry_;
}

compositing::compositing_node_t& mounted_component_t::get_compositing_node() {
  return *compositing_node_;
}

const compositing::compositing_node_t& mounted_component_t::get_compositing_node() const {
  return *compositing_node_;
}

const style::style_base_t& mounted_component_t::get_style() const {
  return style_stack_.get_style_object();
}

style::style_stack& mounted_component_t::get_style_stack() {
  return style_stack_;
}

const backends::frame_base::sptr& mounted_component_t::get_window() const {
  return window_;
}
