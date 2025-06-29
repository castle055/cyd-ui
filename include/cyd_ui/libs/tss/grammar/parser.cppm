/*! \file  parser.cppm
 *! \brief
 *!
 */
module;
#include "bnf_macros.h"

export module cydui.styling.lang.parser;
export import :document;

import std;
export import reflect;
import reflect.serialize;
export import fabric.grammar;


namespace syntax {
  export GRAMMAR(tss)
      : tss_document {
    START(tss_stylesheet);
  };
} // namespace syntax

export namespace lang::tss {
  using parse_result = fabric::parse_result<syntax::tss>;

  template <typename Input>
  parse_result parse(const Input& input_) {
    auto [ok, ast, log] = fabric::parse<syntax::tss>(input_);
    return {ok, ast, log};
  }
} // namespace lang::tss
