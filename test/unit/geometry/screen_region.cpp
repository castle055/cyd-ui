//
// Created by castle on 8/15/24.
//

#include "gtest/gtest.h"

import fabric.logging;
import cydui.screen_region;

using namespace cydui::geometry;


TEST(
  ScreenRegion,
  ClipWith
) {
  screen_region_t region1{0, 0, 100, 100};
  screen_region_t region2{10, 10, 80, 80};

  std::optional<screen_region_t> result = region1.clip_with(region2);

  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result->x, 10);
  EXPECT_EQ(result->y, 10);
  EXPECT_EQ(result->w, 80);
  EXPECT_EQ(result->h, 80);
}

TEST(
  ScreenRegion,
  ClipWith_LeftTopOverflow
) {
  screen_region_t region1{10, 10, 100, 100};
  screen_region_t region2{0, 0, 100, 100};

  std::optional<screen_region_t> result = region1.clip_with(region2);

  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result->x, 10);
  EXPECT_EQ(result->y, 10);
  EXPECT_EQ(result->w, 90);
  EXPECT_EQ(result->h, 90);
}

TEST(
  ScreenRegion,
  ClipWith_RightBottomOverflow
) {
  screen_region_t region1{0, 0, 100, 100};
  screen_region_t region2{10, 10, 100, 100};

  std::optional<screen_region_t> result = region1.clip_with(region2);

  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result->x, 10);
  EXPECT_EQ(result->y, 10);
  EXPECT_EQ(result->w, 90);
  EXPECT_EQ(result->h, 90);
}

TEST(
  ScreenRegion,
  ClipWith_NoClipNeeded
) {
  screen_region_t region1{10, 10, 80, 80};
  screen_region_t region2{0, 0, 100, 100};

  std::optional<screen_region_t> result = region1.clip_with(region2);

  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result->x, 10);
  EXPECT_EQ(result->y, 10);
  EXPECT_EQ(result->w, 80);
  EXPECT_EQ(result->h, 80);
}

TEST(
  ScreenRegion,
  ClipWith_OutOfBounds
) {
  screen_region_t region1{0, 0, 50, 50};
  screen_region_t region2{60, 0, 50, 50};

  std::optional<screen_region_t> result = region1.clip_with(region2);

  ASSERT_FALSE(result.has_value());
}

TEST(
  ScreenRegion,
  ContainsPoint
) {
  screen_region_t region{0, 0, 100, 100};
  bool result = region.contains_point(50, 50);
  ASSERT_TRUE(result);
}

TEST(
  ScreenRegion,
  ContainsPoint_Left
) {
  screen_region_t region{10, 0, 100, 100};
  bool result = region.contains_point(0, 50);
  ASSERT_FALSE(result);
}

TEST(
  ScreenRegion,
  ContainsPoint_Right
) {
  screen_region_t region{0, 0, 100, 100};
  bool result = region.contains_point(150, 50);
  ASSERT_FALSE(result);
}

TEST(
  ScreenRegion,
  ContainsPoint_Top
) {
  screen_region_t region{0, 10, 100, 100};
  bool result = region.contains_point(50, 0);
  ASSERT_FALSE(result);
}

TEST(
  ScreenRegion,
  ContainsPoint_Bottom
) {
  screen_region_t region{0, 0, 100, 100};
  bool result = region.contains_point(50, 150);
  ASSERT_FALSE(result);
}
