// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

export module cydui.components.contexts.events;

import std;
export import reflect;

export template<typename ContextType>
struct ContextUpdate {
  constexpr static const char* type = refl::type_name<ContextUpdate<ContextType>>.data();
  ContextType* ptr = nullptr;
};