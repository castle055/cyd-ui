/*! \file  rules.cppm
 *! \brief
 *!
 */
module;
#include "bnf_macros.h"

export module cydui.styling.lang.parser:rules;
export import :expressions;
export import :selectors;

import std;
export import reflect;
import reflect.serialize;
export import fabric.grammar;
import fabric.grammar.operators;

export import cydui.dimensions;
export import cydui.styling.lang.selectors;
export import cydui.styling.lang.rules;
export import cydui.graphics.types;


namespace syntax {
  export GRAMMAR(tss_rules)
      : tss_expressions,
        tss_selectors {
    ARULE(tss_rule)(~!skip_wn{}, tss_selector{}, !braces_begin{}, tss_declaration_list{}, !braces_end{})(tss::
                                                                                                          StyleRule::
                                                                                                            sptr)({
      $node->data = std::make_shared<tss::StyleRule>();

      if ($node->children[0]->is_type<tss_selector>()) {
        const auto* sel                   = $node->children[0]->as<tss_selector>();
        $node->data->selectors_           = sel->data.selectors;
        $node->data->relevant_components_ = sel->data.selector_keys;
      }
      if ($node->children[1]->is_type<tss_declaration_list>()) {
        const auto* sel          = $node->children[1]->as<tss_declaration_list>();
        $node->data->properties_ = sel->data.archive;
      }
    });
  };
} // namespace syntax
