/*! \file  bnf_macros.h
 *! \brief
 *!
 */

#ifndef BNF_MACROS_H
#define BNF_MACROS_H

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
struct NAME

#define START(RULE) using start = RULE


#endif // BNF_MACROS_H
