//
// Created by castle on 8/15/24.
//

#include "gtest/gtest.h"
#include "parse_utils.hpp"

#define PARSE_EXPR(...) PARSE(syntax::tss_expressions::tss_decl_expression)(__VA_ARGS__)
#define PARSE_LIST(...) PARSE(syntax::tss_expressions::tss_decl_expression_list)(__VA_ARGS__)

TEST(
  Literals,
  Number
) {
  PARSE_EXPR("123.1234");
  ASSERT_EQ(result.data.type().id(), refl::type_id<double>);
  EXPECT_EQ(result.data.as<double>(), 123.1234);
}

TEST(
  Literals,
  Color
) {
  PARSE_EXPR("#112233");
  ASSERT_EQ(result.data.type().id(), refl::type_id<color::Color>);
  EXPECT_EQ(result.data.as<color::Color>(), "#112233"_color);
}

TEST(
  Literals,
  String
) {
  PARSE_EXPR("'hello, world!'");
  ASSERT_EQ(result.data.type().id(), refl::type_id<std::string>);
  EXPECT_EQ(result.data.as<std::string>(), "hello, world!");
}

TEST(
  Literals,
  Identifier
) {
  PARSE_EXPR("some_id");
  ASSERT_EQ(result.data.type().id(), refl::type_id<std::string>);
  EXPECT_EQ(result.data.as<std::string>(), "some_id");
}

TEST(
  Literals,
  Object
) {
  PARSE_EXPR("{ some_var: 123; other_var: 456; }");
  ASSERT_EQ(result.data.type().id(), refl::type_id<refl::archive>);
  refl::archive& archive = result.data.as<refl::archive>();

  EXPECT_TRUE(archive.contains("some_var"));
  EXPECT_TRUE(archive.contains("other_var"));

  refl::any& some_var = archive["some_var"];
  refl::any& other_var = archive["other_var"];

  ASSERT_EQ(some_var.type().id(), refl::type_id<double>);
  ASSERT_EQ(other_var.type().id(), refl::type_id<double>);

  EXPECT_EQ(some_var.as<double>(), 123);
  EXPECT_EQ(other_var.as<double>(), 456);
}

TEST(List, Single) {
  PARSE_LIST("123.1234");
  ASSERT_EQ(result.data.type().id(), refl::type_id<double>);
  EXPECT_EQ(result.data.as<double>(), 123.1234);
}

TEST(List, Multiple) {
  PARSE_LIST("123.1234 123.4321");
  ASSERT_EQ(result.data.type().id(), refl::type_id<std::vector<refl::any>>);
  std::vector<refl::any>& value = result.data.as<std::vector<refl::any>>();

  ASSERT_EQ(value.size(), 2);

  refl::any& val1 = value[0];
  refl::any& val2 = value[1];

  ASSERT_EQ(val1.type().id(), refl::type_id<double>);
  EXPECT_EQ(val1.as<double>(), 123.1234);

  ASSERT_EQ(val2.type().id(), refl::type_id<double>);
  EXPECT_EQ(val2.as<double>(), 123.4321);
}
