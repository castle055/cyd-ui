/*! \file  literals.cppm
 *! \brief
 *!
 */
module;
#include "bnf_macros.h"

export module cydui.styling.lang.parser:literals;
export import :tokens;

import std;
import fabric.logging;
export import reflect;
import reflect.serialize;
export import fabric.grammar;
import fabric.grammar.operators;

export import cydui.dimensions;
export import cydui.styling.lang.selectors;
export import cydui.styling.lang.rules;
export import cydui.graphics.types;


namespace syntax {
  export GRAMMAR(tss_literals)
      : tss_tokens {
    ARULE(tss_decl_color_literal)
    (shebang,
     hex_digit{},
     hex_digit{},
     hex_digit{},
     hex_digit{},
     hex_digit{},
     hex_digit{},
     ~(hex_digit{}, hex_digit{}))(color::Color)({
      $node->data = color::Color::from_str($node->children[0]->text);
    });

    struct number_literal_data {
      double      value;
      std::string unit;
    };
    ARULE(tss_decl_number_literal)(number{}, ~tss_identifier{})(number_literal_data)({
      const std::string& num = $node->children[0]->children[0]->text;
      char*              end;
      $node->data.value = std::strtod(num.c_str(), &end);

      if ($node->children.size() > 1) {
        $node->data.unit = $node->children[1]->text;
      }
    });

    ARULE(tss_decl_string_literal)(tss_single_quoted_str{} | tss_double_quoted_str{})(std::string)({
      $node->data = $node->children[0]->text;
    });
  };
} // namespace syntax
