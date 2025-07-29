// Copyright (c) 2025, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

export module cydui.geometry.configure_anchors;

export import std;
export import cydui.core.Component.impl;

export namespace cydui::layout::anchors {
  void configure_anchors(
    detail::ComponentImpl&                child,
    std::optional<detail::ComponentImpl*> prev
  );
} // namespace cydui::geometry::anchors
