// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module cydui.core.aspects.controller;

import std;
import reflect;

import cydui.core.Component.impl;


using namespace cydui;
using namespace cydui::detail;

std::shared_ptr<void> detail::get_event_handler(Component* component) {
  ComponentImpl* impl = ComponentImpl::from_interface(component);
  return impl->get_event_dispatcher().get_event_handler();
}
