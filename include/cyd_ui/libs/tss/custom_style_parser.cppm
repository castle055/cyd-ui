// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

export module cydui.styling.lang.custom_style_parser;

import std;
export import reflect;

namespace cydui {
  export struct custom_style_parser {
    explicit constexpr custom_style_parser(refl::any (*fun)(const std::string&))
        : parser_function(fun) {}

    refl::any (*parser_function)(const std::string&);
  };
} // namespace cydui
