/*! \file  selectors.cppm
 *! \brief
 *!
 */
module;
#include "bnf_macros.h"

export module cydui.styling.lang.parser:selectors;
export import :tokens;

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
  export GRAMMAR(tss_selectors): tss_tokens {
    RULE(tss_class_selector)            (dot, tss_identifier{});

    RULE(tss_tag_selector)               (shebang, tss_identifier{});

    RULE(tss_pseudo_state_selector)     (colon, tss_identifier{});

    RULE(tss_element_selector)          (tss_identifier{}, *(forward_slash, tss_identifier{}));

    struct selector_data {
      tss::StyleRuleSelector selector{};
    };
    ARULE(tss_descendent_selector_item)  (tss_element_selector{}, *(tss_class_selector{} | tss_tag_selector{} | tss_pseudo_state_selector{}))
    (selector_data)({
      auto& selector = $node->data.selector;

      if (not $node->children.empty()) {
        if ($node->children[0]->is_type<tss_element_selector>()) {
          const auto *es = $node->children[0]->as<tss_element_selector>();
          selector.component = es->children[0]->text;

          std::size_t pos = selector.component.find("/");
          while (pos != std::string::npos) {
            selector.component.replace(pos, 1, "::");
            pos = selector.component.find("/", pos + 2);
          }
        }

        for (std::size_t i = 1; i < $node->children.size(); ++i) {
          auto& child = $node->children[i];
          if (child->is_type<tss_tag_selector>()) {
            const auto *is = child->as<tss_tag_selector>();
            selector.tags.insert(is->children[0]->text.substr(1));
          } else if (child->is_type<tss_pseudo_state_selector>()) {
            const auto *ps = child->as<tss_pseudo_state_selector>();
            selector.pseudo_states.insert(ps->children[0]->text.substr(1));
          }
        }
      }

      // refl::serializer<formats::json_fmt>::to_stream(std::cout, selector);
      // std::cout << std::endl;
    });

    RULE(tss_children_selector_item)     (!right_angle_brackets, !skip_wn{}, tss_descendent_selector_item{});

    struct combined_selector_data {
      tss::StyleRuleCombinedSelector selector{};
      std::string key_element{};
    };
    ARULE(tss_combined_selector)       (tss_descendent_selector_item{}, ~!skip_wn{}, *((tss_children_selector_item{} | tss_descendent_selector_item{}), ~!skip_wn{}))
    (combined_selector_data)({
      auto& selector = $node->data.selector;
      auto& key = $node->data.key_element;

      selector.selectors.emplace_back( //
        tss::StyleRuleCombinedSelector::FIRST_SELECTOR,
        $node->children[0]->as<tss_descendent_selector_item>()->data.selector
      );

      if ($node->children.size() > 1) {
        for (std::size_t i = 1; i < $node->children.size(); ++i) {
          const auto& child = $node->children[i];
          if (child->is_type<tss_children_selector_item>()) {
            const auto *cs = child->as<tss_children_selector_item>()
                     ->children[0]->as<tss_descendent_selector_item>();
            selector.selectors.emplace_back( //
              tss::StyleRuleCombinedSelector::CHILD_COMBINATOR,
              cs->data.selector
            );
          } else if (child->is_type<tss_descendent_selector_item>()) {
            const auto *ss = child->as<tss_descendent_selector_item>();
            selector.selectors.emplace_back( //
              tss::StyleRuleCombinedSelector::DESCENDENT_COMBINATOR,
              ss->data.selector
            );
          }
        }
      }

      key = selector.selectors.back().second.component;
    });

    TRULE(tss_grouping_selector)        (*(tss_combined_selector{}, !comma_separator{}), tss_combined_selector{}, ~!skip_wn{});

    struct selector_list_data {
      std::vector<tss::StyleRuleCombinedSelector> selectors{};
      std::vector<std::string> selector_keys{};
    };
    ARULE(tss_selector)                  (asterisc | tss_grouping_selector{})
    (selector_list_data)({
      for (const auto& child: $node->children) {
        if (child->is_type<tss_combined_selector>()) {
          const auto* cs = child->as<tss_combined_selector>();
          $node->data.selectors.emplace_back(cs->data.selector);
          $node->data.selector_keys.emplace_back(cs->data.key_element);
        }
      }
    });
  };
}
