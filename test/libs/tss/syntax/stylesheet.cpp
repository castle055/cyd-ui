//
// Created by castle on 8/15/24.
//

#include "gtest/gtest.h"
#include "parse_utils.hpp"

using namespace syntax;

TEST(
  Stylesheets,
  Simple
) {
  PARSE(tss_document::tss_stylesheet)("component { somevar: 123; } component { somevar: 123; }");
  EXPECT_EQ(result.data.rules.size(), 2);
}
