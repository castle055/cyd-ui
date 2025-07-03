//
// Created by castle on 8/15/24.
//

#include "gtest/gtest.h"
#include "parse_utils.hpp"

#define PARSE_DECL(...) PARSE(syntax::tss_expressions::tss_declaration)(__VA_ARGS__)

TEST(
  Declarations,
  Simple
) {
  PARSE_DECL("somevar: 123.1234;");
  EXPECT_EQ(result.data.name, "somevar");
  ASSERT_EQ(result.data.value.type().id(), refl::type_id<double>);
  EXPECT_EQ(result.data.value.as<double>(), 123.1234);
}

TEST(
  Declarations,
  PathDeclaration
) {
  PARSE_DECL("someobj.somevar: 123.1234;");
  EXPECT_EQ(result.data.name, "someobj.somevar");
  ASSERT_EQ(result.data.value.type().id(), refl::type_id<double>);
  EXPECT_EQ(result.data.value.as<double>(), 123.1234);
}

TEST(
  Declarations,
  List
) {
  PARSE(syntax::tss_expressions::tss_declaration_list)("somevar: 123.1234; othervar: 'hello, world!';");
  ASSERT_TRUE(result.data.archive.contains("somevar"));
  ASSERT_TRUE(result.data.archive.contains("othervar"));

  refl::any& some_var = result.data.archive["somevar"];
  refl::any& other_var = result.data.archive["othervar"];

  ASSERT_EQ(some_var.type().id(), refl::type_id<double>);
  ASSERT_EQ(other_var.type().id(), refl::type_id<std::string>);

  EXPECT_EQ(some_var.as<double>(), 123.1234);
  EXPECT_EQ(other_var.as<std::string>(), "hello, world!");
}

