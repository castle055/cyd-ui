//
// Created by castle on 8/15/24.
//

#include "gtest/gtest.h"
#include "parse_utils.hpp"

using namespace syntax;

TEST(
  Rules,
  Simple
) {
  PARSE(tss_rules::tss_rule)("component { somevar: 123; }");

  ASSERT_EQ(result.data->relevant_components_.size(), 1);
  EXPECT_EQ(result.data->relevant_components_[0], "component");

  ASSERT_TRUE(result.data->properties_.contains("somevar"));
  ASSERT_EQ(result.data->properties_["somevar"].type().id(), refl::type_id<double>);
  EXPECT_EQ(result.data->properties_["somevar"].as<double>(), 123);
}
