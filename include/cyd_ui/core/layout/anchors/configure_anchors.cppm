// Copyright (c) 2025, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

export module cydui.geometry.configure_anchors;

export import std;
export import cydui.core.mounted;

export namespace cydui::layout::anchors {
  void configure_self_anchors(core::mounted_component_t& child);

  void configure_parent_anchors(core::mounted_component_t& child);

  void configure_prev_anchors(
    core::mounted_component_t&                child,
    std::optional<core::mounted_component_t*> prev
  );

  void configure_anchors(
    core::mounted_component_t&                child,
    std::optional<core::mounted_component_t*> prev
  );
} // namespace cydui::geometry::anchors
