/*! \file  grammar.cppm
 *! \brief 
 *!
 */

export module cydui.styling.tss.parser;

import std;
export import reflect;
import reflect.serialize;
export import fabric.grammar;
import fabric.grammar.operators;

export import cydui.styling.selectors;
export import cydui.styling.rules;
export import cydui.graphics.types;

#define TERMINAL(C) using C##_t = fabric::terminal<#C[0]>; static C##_t C
#define TERMINAL_DIGIT(C) using d##C##_t = fabric::terminal<#C[0]>; static d##C##_t d##C
#define TERMINAL_W_NAME(C, NAME) using NAME##_t = fabric::terminal<C>; static NAME##_t NAME

#define RULE_EXPR__(...) using rule =  decltype(__VA_ARGS__);}
#define RULE(NAME) struct NAME: public fabric::node_t { \
  static constexpr const char* name = #NAME; \
  static constexpr bool transient = false; \
  using sptr = std::shared_ptr<NAME>; \
  NAME(): fabric::node_t(refl::type_id<NAME>) {} \
  RULE_EXPR__

#define TRULE(NAME) struct NAME: public fabric::node_t { \
  static constexpr const char* name = #NAME; \
  static constexpr bool transient = true; \
  using sptr = std::shared_ptr<NAME>; \
  NAME(): fabric::node_t(refl::type_id<NAME>) {} \
  RULE_EXPR__

#define RULE_ACTION__(...) static void action(const sptr& $node) __VA_ARGS__ }
#define ARULE_DATA_TYPE__(...) __VA_ARGS__ data{}; RULE_ACTION__
#define RULE_EXPR_W_ACTION__(...) using rule =  decltype(__VA_ARGS__); ARULE_DATA_TYPE__
#define ARULE(NAME) struct NAME: public fabric::node_t { \
  static constexpr const char* name = #NAME; \
  static constexpr bool transient = false; \
  using sptr = std::shared_ptr<NAME>; \
  NAME(): fabric::node_t(refl::type_id<NAME>) {} \
  RULE_EXPR_W_ACTION__

#define GRAMMAR(NAME) \
struct NAME; \
namespace grammar_metadata { \
  struct NAME { \
    static constexpr const char* name = #NAME; \
  }; \
} \
struct NAME: grammar_metadata::NAME

#define START(RULE) using start = RULE



namespace syntax {
  export GRAMMAR(tss) {
    TERMINAL_DIGIT(0);
    TERMINAL_DIGIT(1);
    TERMINAL_DIGIT(2);
    TERMINAL_DIGIT(3);
    TERMINAL_DIGIT(4);
    TERMINAL_DIGIT(5);
    TERMINAL_DIGIT(6);
    TERMINAL_DIGIT(7);
    TERMINAL_DIGIT(8);
    TERMINAL_DIGIT(9);

    TERMINAL(a);
    TERMINAL(A);
    TERMINAL(b);
    TERMINAL(B);
    TERMINAL(c);
    TERMINAL(C);
    TERMINAL(d);
    TERMINAL(D);
    TERMINAL(e);
    TERMINAL(E);
    TERMINAL(f);
    TERMINAL(F);
    TERMINAL(g);
    TERMINAL(G);
    TERMINAL(h);
    TERMINAL(H);
    TERMINAL(i);
    TERMINAL(I);
    TERMINAL(j);
    TERMINAL(J);
    TERMINAL(k);
    TERMINAL(K);
    TERMINAL(l);
    TERMINAL(L);
    TERMINAL(m);
    TERMINAL(M);
    TERMINAL(n);
    TERMINAL(N);
    TERMINAL(o);
    TERMINAL(O);
    TERMINAL(p);
    TERMINAL(P);
    TERMINAL(q);
    TERMINAL(Q);
    TERMINAL(r);
    TERMINAL(R);
    TERMINAL(s);
    TERMINAL(S);
    TERMINAL(t);
    TERMINAL(T);
    TERMINAL(u);
    TERMINAL(U);
    TERMINAL(v);
    TERMINAL(V);
    TERMINAL(w);
    TERMINAL(W);
    TERMINAL(x);
    TERMINAL(X);
    TERMINAL(y);
    TERMINAL(Y);
    TERMINAL(z);
    TERMINAL(Z);

    TERMINAL_W_NAME(':', colon);
    TERMINAL_W_NAME(';', semicolon);
    TERMINAL_W_NAME('+', plus);
    TERMINAL_W_NAME('-', minus);
    TERMINAL_W_NAME('#', shebang);
    TERMINAL_W_NAME('.', dot);
    TERMINAL_W_NAME(',', comma);
    TERMINAL_W_NAME('*', asterisc);
    TERMINAL_W_NAME(' ', whitespace);
    TERMINAL_W_NAME('\n', newline);
    TERMINAL_W_NAME('\r', return_);

    TERMINAL_W_NAME('[', left_brackets);
    TERMINAL_W_NAME(']', right_brackets);
    TERMINAL_W_NAME('{', left_braces);
    TERMINAL_W_NAME('}', right_braces);
    TERMINAL_W_NAME('(', left_parens);
    TERMINAL_W_NAME(')', right_parens);
    TERMINAL_W_NAME('<', left_angle_brackets);
    TERMINAL_W_NAME('>', right_angle_brackets);
    TERMINAL_W_NAME('/', forward_slash);

    RULE(skip_wn)(*(whitespace | newline | return_));

    RULE(comma_separator)(*(whitespace | newline | return_), comma, *(whitespace | newline | return_));
    RULE(colon_asigner)(*(whitespace | newline | return_), colon, *(whitespace | newline | return_));
    RULE(braces_begin)(*(whitespace | newline | return_), left_braces, *(whitespace | newline | return_));
    RULE(braces_end)(*(whitespace | newline | return_), right_braces, *(whitespace | newline | return_));

    RULE(end_of_declaration)(*(whitespace | newline | return_), semicolon, *(whitespace | newline | return_));

    TRULE(digit_excluding_zero)         (d1 | d2 | d3 | d4 | d5 | d6 | d7 | d8 | d9);

    TRULE(digit)                        (d0 | digit_excluding_zero{});

    TRULE(hex_digit_chars)(
      a | b | c | d | e | f |
      A | B | C | D | E | F);
    TRULE(hex_digit)(hex_digit_chars{} | digit{});

    RULE(number)                        (~(plus | minus), ((*digit{}, dot, +digit{}) | +digit{}));

    TRULE(alpha)                        (a | b | c | d | e | f | g | h | i | j | k | l | m | n | o | p | q | r | s | t | u | v | w | x | y | z );
    TRULE(ALPHA)                        (A | B | C | D | E | F | G | H | I | J | K | L | M | N | O | P | Q | R | S | T | U | V | W | X | Y | Z );

    TRULE(alphanum)                     (alpha{} | ALPHA{} | digit{});

    TRULE(tss_identifier)               ((alpha{} | ALPHA{} | minus), *(alphanum{} | minus));


    // SELECTORS
    RULE(tss_class_selector)            (dot, tss_identifier{});

    RULE(tss_id_selector)               (shebang, tss_identifier{});

    RULE(tss_pseudo_state_selector)     (colon, tss_identifier{});

    RULE(tss_element_selector)          (tss_identifier{}, *(forward_slash, tss_identifier{}));

    struct selector_data {
      cydui::StyleRuleSelector selector{};
    };
    ARULE(tss_descendent_selector_item)  (tss_element_selector{}, *(tss_class_selector{} | tss_id_selector{} | tss_pseudo_state_selector{}))
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
          if (child->is_type<tss_id_selector>()) {
            const auto *is = child->as<tss_id_selector>();
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

    RULE(tss_children_selector_item)     (!left_angle_brackets, tss_descendent_selector_item{});

    struct combined_selector_data {
      cydui::StyleRuleCombinedSelector selector{};
      std::string key_element{};
    };
    ARULE(tss_combined_selector)       (tss_descendent_selector_item{}, !skip_wn{}, *((tss_children_selector_item{} | tss_descendent_selector_item{}), !skip_wn{}))
    (combined_selector_data)({
      auto& selector = $node->data.selector;
      auto& key = $node->data.key_element;

      selector.selectors.emplace_back( //
        cydui::StyleRuleCombinedSelector::FIRST_SELECTOR,
        $node->children[0]->as<tss_descendent_selector_item>()->data.selector
      );

      if ($node->children.size() > 1) {
        for (std::size_t i = 1; i < $node->children.size(); ++i) {
          const auto& child = $node->children[i];
          if (child->is_type<tss_children_selector_item>()) {
            const auto *cs = child->as<tss_children_selector_item>()
                     ->children[0]->as<tss_descendent_selector_item>();
            selector.selectors.emplace_front( //
              cydui::StyleRuleCombinedSelector::CHILD_COMBINATOR,
              cs->data.selector
            );
          } else if (child->is_type<tss_descendent_selector_item>()) {
            const auto *ss = child->as<tss_descendent_selector_item>();
            selector.selectors.emplace_front( //
              cydui::StyleRuleCombinedSelector::DESCENDENT_COMBINATOR,
              ss->data.selector
            );
          }
        }
      }

      key = selector.selectors.back().second.component;
    });

    TRULE(tss_grouping_selector)        (*(tss_combined_selector{}, !comma_separator{}), tss_combined_selector{}, !skip_wn{});

    struct selector_list_data {
      std::vector<cydui::StyleRuleCombinedSelector> selectors{};
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

    // DECLARATIONS
    struct tss_decl_expression_list;

    RULE(tss_decl_function_call)        (tss_identifier{}, left_parens, *(fabric::recurse<tss_decl_expression_list>{}, !comma_separator{}), ~fabric::recurse<tss_decl_expression_list>{}, right_parens);

    ARULE(tss_decl_color_literal)
      (shebang, hex_digit{}, hex_digit{}, hex_digit{}, hex_digit{}, hex_digit{}, hex_digit{},
       ~(hex_digit{}, hex_digit{}))
    (color::Color) ({
      $node->data = color::Color::from_str($node->children[0]->text);
    });

    struct number_literal_data {
      double value;
      std::string unit;
    };
    ARULE(tss_decl_number_literal)       (number{} , ~tss_identifier{})
    (number_literal_data)({
      const std::string& num = $node->children[0]->children[0]->text;
      char* end;
      $node->data.value = std::strtod(num.c_str(), &end);

      if ($node->children.size() > 1) {
        $node->data.unit = $node->children[1]->text;
      }
    });

    ARULE(tss_decl_expression)(tss_decl_color_literal{} | tss_decl_function_call{} | tss_identifier{} | tss_decl_number_literal{})
    (refl::any)({
      const auto& child = $node->children[0];
      refl::any& value = $node->data;

      if (child->is_type<tss_decl_color_literal>()) {
        value = refl::any::make<color::Color>(child->as<tss_decl_color_literal>()->data);
      } else if (child->is_type<tss_decl_function_call>()) {
      } else if (child->is_type<tss_decl_number_literal>()) {
        value = refl::any::make<double>(child->as<tss_decl_number_literal>()->data.value);
      }
    });

    ARULE(tss_decl_expression_list)(*(tss_decl_expression{}, !whitespace, !skip_wn{}), tss_decl_expression{}, !skip_wn{})
    (refl::any)({
      refl::any& value = $node->data;

      if ($node->children.size() == 1) {
        value =  $node->children[0]->as<tss_decl_expression>()->data;
      } else {
        std::vector<refl::any> values{};
        for (const auto& child: $node->children) {
          values.push_back(child->as<tss_decl_expression>()->data);
        }
        const auto& vs = values;
        value = refl::any::make<std::vector<refl::any>>(values);
      }
    });

    RULE(tss_declaration_name) (tss_identifier{});

    struct declaration_data {
      std::string name;
      refl::any value;
    };
    ARULE(tss_declaration)(tss_declaration_name{}, !colon_asigner{}, tss_decl_expression_list{}, !end_of_declaration{})
    (declaration_data)({
      auto& [name, value] = $node->data;
      name = $node->children[0]->children[0]->text;
      value = $node->children[1]->as<tss_decl_expression_list>()->data;
    });

    struct declaration_list_data {
      refl::archive archive{};
    };
    ARULE(tss_declaration_list)(*(tss_declaration{}))
    (declaration_list_data)({
      refl::archive& archive = $node->data.archive;
      for (const auto& decl: $node->children) {
        const auto&[name, value] = decl->as<tss_declaration>()->data;
        archive[name] = value;
      }
    });

    // RULES
    ARULE(tss_rule)(!skip_wn{}, tss_selector{}, !braces_begin{}, tss_declaration_list{}, !braces_end{})
    (cydui::StyleRule::sptr)({
      $node->data = std::make_shared<cydui::StyleRule>();

      if ($node->children[0]->is_type<tss_selector>()) {
        const auto* sel = $node->children[0]->as<tss_selector>();
        $node->data->selectors_ = sel->data.selectors;
        $node->data->relevant_components_ = sel->data.selector_keys;
      }
      if ($node->children[1]->is_type<tss_declaration_list>()) {
        const auto* sel = $node->children[1]->as<tss_declaration_list>();
        $node->data->properties_ = sel->data.archive;
      }
    });

    struct stylesheet_data {
      std::vector<cydui::StyleRule::sptr> rules{};
    };
    ARULE(tss_stylesheet)(*tss_rule())
    (stylesheet_data)({
      for (const auto& child: $node->children) {
        $node->data.rules.push_back(child->as<tss_rule>()->data);
      }
    });

    START(tss_stylesheet);
  };
}

export namespace lang::tss {
  using parse_result = fabric::parse_result<syntax::tss>;

  template <typename Input>
  parse_result parse(const Input& input_) {
    auto [ok, ast, log] = fabric::parse<syntax::tss>(input_);

    return {ok, ast, log};
  }
}
