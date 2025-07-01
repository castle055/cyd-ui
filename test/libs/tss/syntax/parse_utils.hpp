//
// Created by castle on 8/15/24.
//

#ifndef TEST_PARSE_UTILS_H
#define TEST_PARSE_UTILS_H

import reflect.serialize;
import fabric.logging;
import cydui.styling.lang.parser;
import cydui.styling.lang.selectors;

template <typename O>
void print_ast_node(
  O&                    o,
  const fabric::node_t* node,
  int                   indent = 0
) {
  for (int i = 0; i < indent; ++i) {
    o << ' ';
  }
  if (node->is_type<syntax::tss::tss_decl_color_literal>()) {
    auto* c = node->as<syntax::tss::tss_decl_color_literal>();
    o << "<color hex=" << std::format("{:?}", c->data.to_string()) << "/>" << std::endl;
    return;
  }
  if (node->is_text) {
    o << "<text text=" << std::format("{:?}", node->text) << "/>" << std::endl;
    return;
  }

  o << "<" << node->text;
  if (node->children.empty()) {
    o << "/>" << std::endl;
  } else {
    if (node->children.size() == 1 && node->children[0]->is_text) {
      o << " text=" << std::format("{:?}", node->children[0]->text) << "/>" << std::endl;
      return;
    }

    o << ">" << std::endl;
    for (const auto& child: node->children) {
      print_ast_node(o, child.get(), indent + 1);
    }

    for (int i = 0; i < indent; ++i) {
      o << ' ';
    }

    o << "</" << node->text << ">" << std::endl;
  }
}


template <typename Rule>
struct test_grammar {
  using start = Rule;
};

template <typename Rule>
std::optional<std::shared_ptr<typename Rule::start>> parse(const std::string& input) {
  std::println("|------------------------------------------------------------------------------|");
  std::println("# Parser Input:");
  std::println("{}", input);
  std::println();

  auto [ok, ast, log] = fabric::parse<Rule>(input);

  if (ok) {
    std::println("# Parser OK");
    std::cout << std::endl;
  } else {
    std::println("# Parser Error Log:");
    log.dump(std::cout);
    std::cout << std::endl;
  }

  if (not ok) {
    return std::nullopt;
  }

  // std::println("| Parser Output:");
  // std::println("{}", refl::serializer<formats::json_fmt>::to_string(ast->data));
  // std::cout << "| " << std::endl;

  std::println("# AST:");
  print_ast_node(std::cout, ast.get());
  std::cout << std::endl;

  return ast;
}

#define PARSE(RULE) auto result_opt = parse<test_grammar<RULE>> PARSE_INPUT
#define PARSE_INPUT(...)                                                                           \
  (__VA_ARGS__);                                                                                   \
  ASSERT_TRUE(result_opt.has_value()) << std::format("Parser ERROR");                              \
  auto result = **result_opt

#endif // TEST_PARSE_UTILS_H
