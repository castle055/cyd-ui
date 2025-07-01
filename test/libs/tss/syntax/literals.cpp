//
// Created by castle on 8/15/24.
//

#include "gtest/gtest.h"
#include "parse_utils.hpp"

#define PARSE_LITERAL(...) PARSE(syntax::tss_literals::__VA_ARGS__)

TEST(
  Numbers,
  Integer
) {
  PARSE_LITERAL(tss_decl_number_literal)("123");
  EXPECT_EQ(result.data.value, 123);
  EXPECT_TRUE(result.data.unit.empty());
}

TEST(
  Numbers,
  IntegerLeadingZero
) {
  PARSE_LITERAL(tss_decl_number_literal)("0123");
  EXPECT_EQ(result.data.value, 123);
  EXPECT_TRUE(result.data.unit.empty());
}

TEST(
  Numbers,
  IntegerWUnit
) {
  PARSE_LITERAL(tss_decl_number_literal)("123unit");
  EXPECT_EQ(result.data.value, 123);
  EXPECT_EQ(result.data.unit, "unit");
}

TEST(
  Numbers,
  FloatingPoint
) {
  PARSE_LITERAL(tss_decl_number_literal)("123.1234");
  EXPECT_EQ(result.data.value, 123.1234);
  EXPECT_TRUE(result.data.unit.empty());
}

TEST(
  Numbers,
  FloatingPointWUnit
) {
  PARSE_LITERAL(tss_decl_number_literal)("123.1234unit");
  EXPECT_EQ(result.data.value, 123.1234);
  EXPECT_EQ(result.data.unit, "unit");
}

TEST(
  Numbers,
  FloatingPointLessThanOne
) {
  PARSE_LITERAL(tss_decl_number_literal)(".1234");
  EXPECT_EQ(result.data.value, .1234);
  EXPECT_TRUE(result.data.unit.empty());
}

TEST(
  Numbers,
  FloatingPointLessThanOneWUnit
) {
  PARSE_LITERAL(tss_decl_number_literal)(".1234unit");
  EXPECT_EQ(result.data.value, .1234);
  EXPECT_EQ(result.data.unit, "unit");
}

TEST(
  Colors,
  RGB
) {
  PARSE_LITERAL(tss_decl_color_literal)("#112233");
  EXPECT_EQ(result.data, "#112233"_color);
}

TEST(
  Colors,
  RGBA
) {
  PARSE_LITERAL(tss_decl_color_literal)("#11223344");
  EXPECT_EQ(result.data, "#11223344"_color);
}

TEST(
  Strings,
  SingleQuoted
) {
  PARSE_LITERAL(tss_decl_string_literal)("'hello, world!'");
  EXPECT_EQ(result.data, "hello, world!");
}

TEST(
  Strings,
  DoubleQuoted
) {
  PARSE_LITERAL(tss_decl_string_literal)("\"hello, world!\"");
  EXPECT_EQ(result.data, "hello, world!");
}
