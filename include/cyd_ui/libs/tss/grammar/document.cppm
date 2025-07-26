/*! \file  document.cppm
 *! \brief
 *!
 */
module;
#include "bnf_macros.h"

export module cydui.styling.lang.parser:document;
export import :rules;

import std;
export import reflect;
import reflect.serialize;
export import fabric.grammar;
import fabric.grammar.operators;


namespace tss::syntax {
  export GRAMMAR(tss_document)
      : tss_rules {
    struct stylesheet_data {
      std::vector<tss::StyleRule::sptr> rules{};
    };
    ARULE(tss_stylesheet) (*tss_rule())(stylesheet_data)({
      for (const auto& child: $node->children) {
        $node->data.rules.push_back(child->as<tss_rule>()->data);
      }
    });
  };
} // namespace syntax
