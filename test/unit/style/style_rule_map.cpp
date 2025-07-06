//
// Created by castle on 8/15/24.
//

#include "gtest/gtest.h"

import fabric.logging;
import cydui.styling.style_rule_map;

using namespace cydui::style;


struct test_obj_t: style_base_t {
  int    a = 1;
  double x = 2.0;
};

const refl::type_info& test_type_info = refl::type_info::from<test_obj_t>();

const auto           style_obj_ptr = std::make_shared<test_obj_t>();
const style_object_t style_obj{style_obj_ptr};

TEST(
  StyleRuleMap,
  Constructor
) {
  style_rule_map rules{test_type_info};

  EXPECT_EQ(rules.count_applicable_rules(), 0);
  EXPECT_EQ(rules.count_active_rules(), 0);
  EXPECT_EQ(rules.get_style_map().get_type().name(), test_type_info.name());
  EXPECT_TRUE(rules.get_style_map().empty());
}

std::vector<tss::StyleRuleInstance> parse_rules(
  std::string                                                      str,
  std::function<bool(const std::shared_ptr<tss::StyleRule>& rule)> pred =
    [](const std::shared_ptr<tss::StyleRule>&) { return true; }
) {
  auto stylesheet   = tss::StyleSheet::parse(str);
  auto parsed_rules = stylesheet->rules_by_component("component");


  std::vector<tss::StyleRuleInstance> style_rules{};
  for (const auto& rule: parsed_rules) {
    if (pred(rule)) {
      style_rules.emplace_back(rule->selectors_.front().specificity(), rule);
    }
  }

  std::stable_sort(
    style_rules.begin(),
    style_rules.end(),
    [](const tss::StyleRuleInstance& lhs, const tss::StyleRuleInstance& rhs) {
      return lhs.specificity < rhs.specificity;
    }
  );

  return style_rules;
}


TEST(
  StyleRuleMap,
  UpdateRuleList
) {
  style_rule_map rules{test_type_info};

  std::vector<tss::StyleRuleInstance> style_rules = parse_rules("component { a: 123; }");

  bool changed = rules.update_rule_list(style_rules);
  EXPECT_TRUE(changed);

  EXPECT_EQ(rules.count_applicable_rules(), 1);
  EXPECT_EQ(rules.count_active_rules(), 0);
  EXPECT_TRUE(rules.get_style_map().empty());
}

TEST(
  StyleRuleMap,
  UpdateRuleList_NoChange
) {
  style_rule_map rules{test_type_info};

  std::vector<tss::StyleRuleInstance> style_rules = parse_rules("");

  bool changed = rules.update_rule_list(style_rules);
  EXPECT_FALSE(changed);

  EXPECT_EQ(rules.count_applicable_rules(), 0);
  EXPECT_EQ(rules.count_active_rules(), 0);
  EXPECT_TRUE(rules.get_style_map().empty());
}

TEST(
  StyleRuleMap,
  UpdateRuleList_NestedChange
) {
  style_rule_map rules{test_type_info};

  std::vector<tss::StyleRuleInstance> style_rules1 = parse_rules("component { a: 123; }");
  std::vector<tss::StyleRuleInstance> style_rules2 = parse_rules("component { a: 321; }");

  bool changed = rules.update_rule_list(style_rules1);
  EXPECT_TRUE(changed);
  changed = rules.update_rule_list(style_rules2);
  EXPECT_TRUE(changed);

  EXPECT_EQ(rules.count_applicable_rules(), 1);
  EXPECT_EQ(rules.count_active_rules(), 0);
  EXPECT_TRUE(rules.get_style_map().empty());
}

TEST(
  StyleRuleMap,
  ManageRules
) {
  style_rule_map rules{test_type_info};

  std::vector<tss::StyleRuleInstance> style_rules = parse_rules("component { a: 123; }");
  rules.update_rule_list(style_rules);

  bool changed = rules.manage_rules([](const auto& rule) { return true; });
  EXPECT_TRUE(changed);

  EXPECT_EQ(rules.count_applicable_rules(), 1);
  EXPECT_EQ(rules.count_active_rules(), 1);
  ASSERT_FALSE(rules.get_style_map().empty());

  auto value_opt = rules.get_style_map().get_field("a");
  ASSERT_TRUE(value_opt.has_value());
  ASSERT_EQ(value_opt.value().type().name(), refl::type_name<int>);
  EXPECT_EQ(value_opt.value().as<int>(), 123);
}

TEST(
  StyleRuleMap,
  ManageRules_DeactivateRule
) {
  style_rule_map rules{test_type_info};

  std::vector<tss::StyleRuleInstance> style_rules = parse_rules("component { a: 123; }");
  rules.update_rule_list(style_rules);

  bool changed = rules.manage_rules([](const auto& rule) { return true; });
  EXPECT_TRUE(changed);
  EXPECT_EQ(rules.count_applicable_rules(), 1);
  EXPECT_EQ(rules.count_active_rules(), 1);
  ASSERT_FALSE(rules.get_style_map().empty());

  changed = rules.manage_rules([](const auto& rule) { return false; });
  EXPECT_TRUE(changed);

  EXPECT_EQ(rules.count_applicable_rules(), 1);
  EXPECT_EQ(rules.count_active_rules(), 0);
  ASSERT_TRUE(rules.get_style_map().empty());
}

TEST(
  StyleRuleMap,
  ManageRules_NoChange
) {
  style_rule_map rules{test_type_info};

  std::vector<tss::StyleRuleInstance> style_rules = parse_rules("component { a: 123; }");
  rules.update_rule_list(style_rules);

  bool changed = rules.manage_rules([](const auto& rule) { return true; });
  EXPECT_TRUE(changed);
  EXPECT_EQ(rules.count_applicable_rules(), 1);
  EXPECT_EQ(rules.count_active_rules(), 1);
  ASSERT_FALSE(rules.get_style_map().empty());

  changed = rules.manage_rules([](const auto& rule) { return true; });
  EXPECT_FALSE(changed);

  EXPECT_EQ(rules.count_applicable_rules(), 1);
  EXPECT_EQ(rules.count_active_rules(), 1);
  ASSERT_FALSE(rules.get_style_map().empty());

  auto value_opt = rules.get_style_map().get_field("a");
  ASSERT_TRUE(value_opt.has_value());
  ASSERT_EQ(value_opt.value().type().name(), refl::type_name<int>);
  EXPECT_EQ(value_opt.value().as<int>(), 123);
}

TEST(
  StyleRuleMap,
  Projection_OverrideValue
) {
  style_rule_map rules{test_type_info};

  std::vector<tss::StyleRuleInstance> style_rules =
    parse_rules("component { a: 123; } component { a: 321; }");
  rules.update_rule_list(style_rules);

  bool changed = rules.manage_rules([](const auto& rule) { return true; });
  EXPECT_TRUE(changed);
  EXPECT_EQ(rules.count_applicable_rules(), 2);
  EXPECT_EQ(rules.count_active_rules(), 2);
  ASSERT_FALSE(rules.get_style_map().empty());

  auto value_opt = rules.get_style_map().get_field("a");
  ASSERT_TRUE(value_opt.has_value());
  ASSERT_EQ(value_opt.value().type().name(), refl::type_name<int>);
  EXPECT_EQ(value_opt.value().as<int>(), 321);
}

TEST(
  StyleRuleMap,
  Projection_Specifity
) {
  style_rule_map rules{test_type_info};

  std::vector<tss::StyleRuleInstance> style_rules =
    parse_rules("component#some-tag { a: 123; } component { a: 321; }");
  rules.update_rule_list(style_rules);

  bool changed = rules.manage_rules([](const auto& rule) { return true; });
  EXPECT_TRUE(changed);
  EXPECT_EQ(rules.count_applicable_rules(), 2);
  EXPECT_EQ(rules.count_active_rules(), 2);
  ASSERT_FALSE(rules.get_style_map().empty());

  auto value_opt = rules.get_style_map().get_field("a");
  ASSERT_TRUE(value_opt.has_value());
  ASSERT_EQ(value_opt.value().type().name(), refl::type_name<int>);
  EXPECT_EQ(value_opt.value().as<int>(), 123);
}

TEST(
  StyleRuleMap,
  Projection_TiedSpecifity
) {
  style_rule_map rules{test_type_info};

  std::vector<tss::StyleRuleInstance> style_rules =
    parse_rules("component#some-tag { a: 123; } component:some-state { a: 321; }");
  rules.update_rule_list(style_rules);

  bool changed = rules.manage_rules([](const auto& rule) { return true; });
  EXPECT_TRUE(changed);
  EXPECT_EQ(rules.count_applicable_rules(), 2);
  EXPECT_EQ(rules.count_active_rules(), 2);
  ASSERT_FALSE(rules.get_style_map().empty());

  auto value_opt = rules.get_style_map().get_field("a");
  ASSERT_TRUE(value_opt.has_value());
  ASSERT_EQ(value_opt.value().type().name(), refl::type_name<int>);
  EXPECT_EQ(value_opt.value().as<int>(), 321);
}
