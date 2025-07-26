/*! \file  expressions.cppm
 *! \brief
 *!
 */
module;
#include "bnf_macros.h"

export module cydui.styling.lang.parser:expressions;
export import :literals;

import std;
export import reflect;
import reflect.serialize;
export import fabric.grammar;
import fabric.grammar.operators;

export import cydui.dimensions;
export import cydui.styling.lang.selectors;
export import cydui.styling.lang.rules;
export import cydui.graphics.types;


namespace tss::syntax {
  export GRAMMAR(tss_expressions)
      : tss_literals {
    struct tss_decl_expression_list;
    struct tss_obj_expression;

    RULE(tss_decl_function_call)(
      tss_identifier{},
      left_parens,
      *(fabric::recurse<tss_decl_expression_list>{}, !comma_separator{}),
      ~fabric::recurse<tss_decl_expression_list>{},
      right_parens
    );


    ARULE(tss_decl_expression) //
    (fabric::recurse<tss_obj_expression>{} | tss_decl_color_literal{} | tss_decl_function_call{}
     | tss_identifier{} | tss_decl_number_literal{} | tss_decl_string_literal{})(refl::any)({
      const auto& child = $node->children[0];
      refl::any&  value = $node->data;

      if (child->is_type<tss_decl_color_literal>()) {
        value = refl::any::make<cydui::Color>(child->as<tss_decl_color_literal>()->data);
      } else if (child->is_type<tss_decl_function_call>()) {
      } else if (child->is_type<tss_obj_expression>()) {
        value = child->as<tss_obj_expression>()->data;
      } else if (child->is_type<tss_decl_string_literal>()) {
        value = refl::any::make<std::string>(child->as<tss_decl_string_literal>()->data);
      } else if (child->is_type<tss_decl_number_literal>()) {
        auto d = child->as<tss_decl_number_literal>()->data;
        if (d.unit.empty()) {
          value = refl::any::make<double>(d.value);
        } else if (d.unit == "px") {
          value = refl::any::make<cydui::dimensions::screen_measure>(d.value);
        }
      } else if (not child->text.empty()) {
        value = refl::any::make<std::string>(child->text);
      }
    });

    ARULE(tss_decl_expression_list) //
    (*(tss_decl_expression{}, !whitespace, !skip_wn{}),
     tss_decl_expression{},
     ~!skip_wn{})(refl::any)({
      refl::any& value = $node->data;

      if ($node->children.size() == 1) {
        value = $node->children[0]->as<tss_decl_expression>()->data;
      } else {
        std::vector<refl::any> values{};
        for (const auto& child: $node->children) {
          values.push_back(child->as<tss_decl_expression>()->data);
        }
        value = refl::any::make<std::vector<refl::any>>(values);
      }
    });

    RULE(tss_declaration_name)(tss_identifier{}, *(dot, tss_identifier{}));

    struct declaration_data {
      std::string name;
      refl::any   value;
    };
    ARULE(tss_declaration)(tss_declaration_name{}, !colon_asigner{}, tss_decl_expression_list{}, !end_of_declaration{})(declaration_data)({
      auto& [name, value] = $node->data;
      name                = $node->children[0]->children[0]->text;
      value               = $node->children[1]->as<tss_decl_expression_list>()->data;
    });

    struct declaration_list_data {
      refl::archive archive{};
    };
    ARULE(tss_declaration_list) (*(tss_declaration{}))(declaration_list_data)({
      refl::archive& archive = $node->data.archive;
      for (const auto& decl: $node->children) {
        const auto& [name, value] = decl->as<tss_declaration>()->data;
        archive[name]             = value;
      }
    });

    ARULE(tss_obj_expression)(!braces_begin{}, tss_declaration_list{}, !braces_end{})(refl::any)({
      if ($node->children[0]->is_type<tss_declaration_list>()) {
        const auto* list = $node->children[0]->as<tss_declaration_list>();
        $node->data      = list->data.archive;
      }
    });
  };
} // namespace syntax
