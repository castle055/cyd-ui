//
// Created by castle on 8/15/24.
//

#include "gtest/gtest.h"
#include "parse_utils.hpp"

using namespace syntax;

TEST(
  SelectorElements,
  ClassSelector
) {
  PARSE(tss_selectors::tss_class_selector)(".some-class");
  ASSERT_FALSE(result.children.empty());
  EXPECT_EQ(result.children[0]->text, ".some-class");
}

TEST(
  SelectorElements,
  TagSelector
) {
  PARSE(tss_selectors::tss_tag_selector)("#some-tag");
  ASSERT_FALSE(result.children.empty());
  EXPECT_EQ(result.children[0]->text, "#some-tag");
}

TEST(
  SelectorElements,
  PseudoStateSelector
) {
  PARSE(tss_selectors::tss_pseudo_state_selector)(":some-state");
  ASSERT_FALSE(result.children.empty());
  EXPECT_EQ(result.children[0]->text, ":some-state");
}

TEST(
  SelectorElements,
  ComponentSelector
) {
  PARSE(tss_selectors::tss_element_selector)("component");
  ASSERT_FALSE(result.children.empty());
  EXPECT_EQ(result.children[0]->text, "component");
}

TEST(
  SelectorElements,
  NamespacedComponentSelector
) {
  PARSE(tss_selectors::tss_element_selector)("some/namespaced/component");
  ASSERT_FALSE(result.children.empty());
  EXPECT_EQ(result.children[0]->text, "some/namespaced/component");
}

TEST(
  Selectors,
  Simple
) {
  PARSE(tss_selectors::tss_selector)("component");
  auto& selector_list = result.data;
  ASSERT_FALSE(selector_list.selectors.empty());
  ASSERT_FALSE(selector_list.selector_keys.empty());

  EXPECT_EQ(selector_list.selectors.size(), 1);
  EXPECT_EQ(selector_list.selector_keys.size(), 1);

  EXPECT_EQ(selector_list.selectors[0].selectors.size(), 1);
  EXPECT_EQ(
    selector_list.selectors[0].selectors.front().first,
    ::tss::StyleRuleCombinedSelector::FIRST_SELECTOR
  );
  EXPECT_EQ(selector_list.selectors[0].selectors.front().second.component, "component");
  EXPECT_TRUE(selector_list.selectors[0].selectors.front().second.pseudo_states.empty());
  EXPECT_TRUE(selector_list.selectors[0].selectors.front().second.tags.empty());

  EXPECT_EQ(selector_list.selector_keys[0], "component");
}

TEST(
  Selectors,
  WithPseudoState
) {
  PARSE(tss_selectors::tss_selector)("component:somestate");
  auto& selector_list = result.data;
  ASSERT_FALSE(selector_list.selectors.empty());
  ASSERT_FALSE(selector_list.selector_keys.empty());

  EXPECT_EQ(selector_list.selectors.size(), 1);
  EXPECT_EQ(selector_list.selector_keys.size(), 1);

  EXPECT_EQ(selector_list.selectors[0].selectors.size(), 1);
  EXPECT_EQ(
    selector_list.selectors[0].selectors.front().first,
    ::tss::StyleRuleCombinedSelector::FIRST_SELECTOR
  );
  EXPECT_EQ(selector_list.selectors[0].selectors.front().second.component, "component");

  ASSERT_FALSE(selector_list.selectors[0].selectors.front().second.pseudo_states.empty());
  EXPECT_EQ(selector_list.selectors[0].selectors.front().second.pseudo_states.size(), 1);
  EXPECT_TRUE(
    selector_list.selectors[0].selectors.front().second.pseudo_states.contains("somestate")
  );

  EXPECT_TRUE(selector_list.selectors[0].selectors.front().second.tags.empty());

  EXPECT_EQ(selector_list.selector_keys[0], "component");
}

TEST(
  Selectors,
  WithTag
) {
  PARSE(tss_selectors::tss_selector)("component#sometag");
  auto& selector_list = result.data;
  ASSERT_FALSE(selector_list.selectors.empty());
  ASSERT_FALSE(selector_list.selector_keys.empty());

  EXPECT_EQ(selector_list.selectors.size(), 1);
  EXPECT_EQ(selector_list.selector_keys.size(), 1);

  EXPECT_EQ(selector_list.selectors[0].selectors.size(), 1);
  EXPECT_EQ(
    selector_list.selectors[0].selectors.front().first,
    ::tss::StyleRuleCombinedSelector::FIRST_SELECTOR
  );
  EXPECT_EQ(selector_list.selectors[0].selectors.front().second.component, "component");

  EXPECT_TRUE(selector_list.selectors[0].selectors.front().second.pseudo_states.empty());

  ASSERT_FALSE(selector_list.selectors[0].selectors.front().second.tags.empty());
  EXPECT_EQ(selector_list.selectors[0].selectors.front().second.tags.size(), 1);
  EXPECT_TRUE(selector_list.selectors[0].selectors.front().second.tags.contains("sometag"));

  EXPECT_EQ(selector_list.selector_keys[0], "component");
}

TEST(
  Selectors,
  NamespacedSimple
) {
  PARSE(tss_selectors::tss_selector)("some/namespace/component");
  auto& selector_list = result.data;
  ASSERT_FALSE(selector_list.selectors.empty());
  ASSERT_FALSE(selector_list.selector_keys.empty());

  EXPECT_EQ(selector_list.selectors.size(), 1);
  EXPECT_EQ(selector_list.selector_keys.size(), 1);

  EXPECT_EQ(selector_list.selectors[0].selectors.size(), 1);
  EXPECT_EQ(
    selector_list.selectors[0].selectors.front().first,
    ::tss::StyleRuleCombinedSelector::FIRST_SELECTOR
  );
  EXPECT_EQ(
    selector_list.selectors[0].selectors.front().second.component, "some::namespace::component"
  );
  EXPECT_TRUE(selector_list.selectors[0].selectors.front().second.pseudo_states.empty());
  EXPECT_TRUE(selector_list.selectors[0].selectors.front().second.tags.empty());

  EXPECT_EQ(selector_list.selector_keys[0], "some::namespace::component");
}

TEST(
  Selectors,
  NamespacedWithPseudoState
) {
  PARSE(tss_selectors::tss_selector)("some/namespace/component:somestate");
  auto& selector_list = result.data;
  ASSERT_FALSE(selector_list.selectors.empty());
  ASSERT_FALSE(selector_list.selector_keys.empty());

  EXPECT_EQ(selector_list.selectors.size(), 1);
  EXPECT_EQ(selector_list.selector_keys.size(), 1);

  EXPECT_EQ(selector_list.selectors[0].selectors.size(), 1);
  EXPECT_EQ(
    selector_list.selectors[0].selectors.front().first,
    ::tss::StyleRuleCombinedSelector::FIRST_SELECTOR
  );
  EXPECT_EQ(
    selector_list.selectors[0].selectors.front().second.component, "some::namespace::component"
  );

  ASSERT_FALSE(selector_list.selectors[0].selectors.front().second.pseudo_states.empty());
  EXPECT_EQ(selector_list.selectors[0].selectors.front().second.pseudo_states.size(), 1);
  EXPECT_TRUE(
    selector_list.selectors[0].selectors.front().second.pseudo_states.contains("somestate")
  );

  EXPECT_TRUE(selector_list.selectors[0].selectors.front().second.tags.empty());

  EXPECT_EQ(selector_list.selector_keys[0], "some::namespace::component");
}

TEST(
  Selectors,
  NamespacedWithTag
) {
  PARSE(tss_selectors::tss_selector)("some/namespace/component#sometag");
  auto& selector_list = result.data;
  ASSERT_FALSE(selector_list.selectors.empty());
  ASSERT_FALSE(selector_list.selector_keys.empty());

  EXPECT_EQ(selector_list.selectors.size(), 1);
  EXPECT_EQ(selector_list.selector_keys.size(), 1);

  EXPECT_EQ(selector_list.selectors[0].selectors.size(), 1);
  EXPECT_EQ(
    selector_list.selectors[0].selectors.front().first,
    ::tss::StyleRuleCombinedSelector::FIRST_SELECTOR
  );
  EXPECT_EQ(
    selector_list.selectors[0].selectors.front().second.component, "some::namespace::component"
  );

  EXPECT_TRUE(selector_list.selectors[0].selectors.front().second.pseudo_states.empty());

  ASSERT_FALSE(selector_list.selectors[0].selectors.front().second.tags.empty());
  EXPECT_EQ(selector_list.selectors[0].selectors.front().second.tags.size(), 1);
  EXPECT_TRUE(selector_list.selectors[0].selectors.front().second.tags.contains("sometag"));

  EXPECT_EQ(selector_list.selector_keys[0], "some::namespace::component");
}

TEST(
  CombinedSelectors,
  SimpleDescendent
) {
  PARSE(tss_selectors::tss_selector)("componentA componentB");
  auto& selector_list = result.data;
  ASSERT_FALSE(selector_list.selectors.empty());
  ASSERT_FALSE(selector_list.selector_keys.empty());

  EXPECT_EQ(selector_list.selectors.size(), 1);
  EXPECT_EQ(selector_list.selector_keys.size(), 1);

  auto& selector = selector_list.selectors[0];

  ASSERT_EQ(selector.selectors.size(), 2);
  EXPECT_EQ(selector.selectors.begin()->first, ::tss::StyleRuleCombinedSelector::FIRST_SELECTOR);
  EXPECT_EQ(selector.selectors.begin()->second.component, "componentA");
  EXPECT_EQ(
    (++selector.selectors.begin())->first, ::tss::StyleRuleCombinedSelector::DESCENDENT_COMBINATOR
  );
  EXPECT_EQ((++selector.selectors.begin())->second.component, "componentB");

  EXPECT_TRUE((++selector.selectors.begin())->second.pseudo_states.empty());
  EXPECT_TRUE((++selector.selectors.begin())->second.tags.empty());
  EXPECT_TRUE(selector.selectors.begin()->second.pseudo_states.empty());
  EXPECT_TRUE(selector.selectors.begin()->second.tags.empty());

  EXPECT_EQ(selector_list.selector_keys[0], "componentB");
}

TEST(
  CombinedSelectors,
  SimpleChildren
) {
  PARSE(tss_selectors::tss_selector)("componentA > componentB");
  auto& selector_list = result.data;
  ASSERT_FALSE(selector_list.selectors.empty());
  ASSERT_FALSE(selector_list.selector_keys.empty());

  EXPECT_EQ(selector_list.selectors.size(), 1);
  EXPECT_EQ(selector_list.selector_keys.size(), 1);

  auto& selector = selector_list.selectors[0];

  ASSERT_EQ(selector.selectors.size(), 2);
  EXPECT_EQ(selector.selectors.begin()->first, ::tss::StyleRuleCombinedSelector::FIRST_SELECTOR);
  EXPECT_EQ(selector.selectors.begin()->second.component, "componentA");
  EXPECT_EQ(
    (++selector.selectors.begin())->first, ::tss::StyleRuleCombinedSelector::CHILD_COMBINATOR
  );
  EXPECT_EQ((++selector.selectors.begin())->second.component, "componentB");

  EXPECT_TRUE((++selector.selectors.begin())->second.pseudo_states.empty());
  EXPECT_TRUE((++selector.selectors.begin())->second.tags.empty());
  EXPECT_TRUE(selector.selectors.begin()->second.pseudo_states.empty());
  EXPECT_TRUE(selector.selectors.begin()->second.tags.empty());

  EXPECT_EQ(selector_list.selector_keys[0], "componentB");
}

TEST(
  GroupedSelectors,
  Simple
) {
  PARSE(tss_selectors::tss_selector)("componentA, componentB");
  auto& selector_list = result.data;
  ASSERT_FALSE(selector_list.selectors.empty());
  ASSERT_FALSE(selector_list.selector_keys.empty());

  EXPECT_EQ(selector_list.selectors.size(), 2);
  EXPECT_EQ(selector_list.selector_keys.size(), 2);

  EXPECT_EQ(selector_list.selectors[0].selectors.size(), 1);
  EXPECT_EQ(
    selector_list.selectors[0].selectors.front().first,
    ::tss::StyleRuleCombinedSelector::FIRST_SELECTOR
  );
  EXPECT_EQ(selector_list.selectors[0].selectors.front().second.component, "componentA");

  EXPECT_EQ(selector_list.selectors[1].selectors.size(), 1);
  EXPECT_EQ(
    selector_list.selectors[1].selectors.front().first,
    ::tss::StyleRuleCombinedSelector::FIRST_SELECTOR
  );
  EXPECT_EQ(selector_list.selectors[1].selectors.front().second.component, "componentB");

  EXPECT_TRUE(selector_list.selectors[0].selectors.front().second.pseudo_states.empty());
  EXPECT_TRUE(selector_list.selectors[0].selectors.front().second.tags.empty());
  EXPECT_TRUE(selector_list.selectors[1].selectors.front().second.pseudo_states.empty());
  EXPECT_TRUE(selector_list.selectors[1].selectors.front().second.tags.empty());

  EXPECT_EQ(selector_list.selector_keys[0], "componentA");
  EXPECT_EQ(selector_list.selector_keys[1], "componentB");
}
