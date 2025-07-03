//
// Created by castle on 8/15/24.
//

#include "gtest/gtest.h"

import fabric.logging;
import cydui.styling.sparse_style_map;

using namespace cydui::style;

struct inner_test_obj_t {
  int c = 5;
};

// Used to test custom conversions
struct custom_type {
  std::string str{"default"};
};
struct custom_type2 {
  std::string str{"default"};
};
struct custom_type3 {
  std::string str{"default"};
};

struct test_obj_t: style_base_t {
  int         a = 1;
  double      x = 2.0;
  std::string s = "hello world";

  [[meta(cydui::CustomConversion::from<int>([](const refl::any& it) {
    return refl::any::make<custom_type>({.str = std::format("VAL:{}", it.as<int>())});
  }))]]
  custom_type custom{};

  custom_type2 custom2{};
  custom_type3 custom3{};

  inner_test_obj_t o{};
};

const refl::type_info& test_type_info = refl::type_info::from<test_obj_t>();

TEST(
  SparseStyleMap,
  Constructor
) {
  sparse_style_map style_map{test_type_info};
  EXPECT_EQ(style_map.size(), 0);
  EXPECT_EQ(style_map.get_type().id(), test_type_info.id());
  EXPECT_TRUE(style_map.empty());
}

TEST(
  SparseStyleMap,
  FindPath
) {
  sparse_style_map style_map{test_type_info};

  std::optional<refl::field_path> path_opt = style_map.find_path("a");
  ASSERT_TRUE(path_opt.has_value());
  EXPECT_EQ(path_opt.value().root_type().id(), refl::type_id<test_obj_t>);
  EXPECT_EQ(path_opt.value().type().id(), refl::type_id<int>);
}

TEST(
  SparseStyleMap,
  FindPath_BaseStyle
) {
  sparse_style_map style_map{test_type_info};

  std::optional<refl::field_path> path_opt = style_map.find_path("width");
  ASSERT_TRUE(path_opt.has_value());
  EXPECT_EQ(path_opt.value().root_type().id(), refl::type_id<cydui::style::style_base_t>);
  EXPECT_EQ(path_opt.value().type().id(), refl::type_id<cydui::expression_t>);
}

TEST(
  SparseStyleMap,
  FindPath_BaseStyleHasPriority
) {
  sparse_style_map style_map{test_type_info};

  std::optional<refl::field_path> path_opt = style_map.find_path("x");
  ASSERT_TRUE(path_opt.has_value());
  EXPECT_EQ(path_opt.value().root_type().id(), refl::type_id<cydui::style::style_base_t>);
  EXPECT_EQ(path_opt.value().type().id(), refl::type_id<cydui::expression_t>);
}

TEST(
  SparseStyleMap,
  FindPath_NotFound
) {
  sparse_style_map style_map{test_type_info};

  std::optional<refl::field_path> path_opt = style_map.find_path("not_a_field");
  ASSERT_FALSE(path_opt.has_value());
}

TEST(
  SparseStyleMap,
  SetAndGetField_BaseStyle
) {
  sparse_style_map style_map{test_type_info};

  static cydui::position_e test_value = cydui::position_e::ABSOLUTE;
  bool changed = style_map.set_field("position", refl::any::make<cydui::position_e>(test_value));
  EXPECT_TRUE(changed);
  EXPECT_FALSE(style_map.empty());
  EXPECT_EQ(style_map.size(), 1);

  std::optional<refl::any_ref> value = style_map.get_field("position");
  ASSERT_TRUE(value.has_value());
  EXPECT_EQ(value.value().type().id(), refl::type_id<cydui::position_e>);
  EXPECT_EQ(value.value().as<cydui::position_e>(), test_value);
}

TEST(
  SparseStyleMap,
  SetAndGetField_CustomStyle
) {
  sparse_style_map style_map{test_type_info};

  static int test_value = 12345;
  bool       changed    = style_map.set_field("a", refl::any::make<int>(test_value));
  EXPECT_TRUE(changed);
  EXPECT_FALSE(style_map.empty());
  EXPECT_EQ(style_map.size(), 1);

  std::optional<refl::any_ref> value = style_map.get_field("a");
  ASSERT_TRUE(value.has_value());
  EXPECT_EQ(value.value().type().id(), refl::type_id<int>);
  EXPECT_EQ(value.value().as<int>(), test_value);
}

TEST(
  SparseStyleMap,
  SetAndConstGetField_BaseStyle
) {
  sparse_style_map        mut_style_map{test_type_info};
  const sparse_style_map& style_map{mut_style_map};

  static cydui::position_e test_value = cydui::position_e::ABSOLUTE;
  bool                     changed =
    mut_style_map.set_field("position", refl::any::make<cydui::position_e>(test_value));
  EXPECT_TRUE(changed);
  EXPECT_FALSE(style_map.empty());
  EXPECT_EQ(style_map.size(), 1);

  std::optional<refl::any> value = style_map.get_field("position");
  ASSERT_TRUE(value.has_value());
  EXPECT_EQ(value.value().type().id(), refl::type_id<cydui::position_e>);
  EXPECT_EQ(value.value().as<cydui::position_e>(), test_value);
}

TEST(
  SparseStyleMap,
  SetAndConstGetField_CustomStyle
) {
  sparse_style_map        mut_style_map{test_type_info};
  const sparse_style_map& style_map{mut_style_map};

  static int test_value = 12345;
  bool       changed    = mut_style_map.set_field("a", refl::any::make<int>(test_value));
  EXPECT_TRUE(changed);
  EXPECT_FALSE(style_map.empty());
  EXPECT_EQ(style_map.size(), 1);

  std::optional<refl::any> value = style_map.get_field("a");
  ASSERT_TRUE(value.has_value());
  EXPECT_EQ(value.value().type().id(), refl::type_id<int>);
  EXPECT_EQ(value.value().as<int>(), test_value);
}

TEST(
  SparseStyleMap,
  SetField_InvalidPath
) {
  sparse_style_map style_map{test_type_info};

  static int test_value = 12345;
  bool       changed    = style_map.set_field("not_a_field", refl::any::make<int>(test_value));
  EXPECT_FALSE(changed);
  EXPECT_TRUE(style_map.empty());
  EXPECT_EQ(style_map.size(), 0);
}

TEST(
  SparseStyleMap,
  GetField_NotFound
) {
  sparse_style_map style_map{test_type_info};

  std::optional<refl::any_ref> value = style_map.get_field("position");
  ASSERT_FALSE(value.has_value());
}

TEST(
  SparseStyleMap,
  GetField_InvalidPath
) {
  sparse_style_map style_map{test_type_info};

  static cydui::position_e test_value = cydui::position_e::ABSOLUTE;
  bool changed = style_map.set_field("position", refl::any::make<cydui::position_e>(test_value));
  EXPECT_TRUE(changed);
  EXPECT_FALSE(style_map.empty());
  EXPECT_EQ(style_map.size(), 1);

  std::optional<refl::any_ref> value = style_map.get_field("not_a_field");
  ASSERT_FALSE(value.has_value());
}

TEST(
  SparseStyleMap,
  ConstGetField_NotFound
) {
  sparse_style_map        mut_style_map{test_type_info};
  const sparse_style_map& style_map{mut_style_map};

  std::optional<refl::any> value = style_map.get_field("position");
  ASSERT_FALSE(value.has_value());
}

TEST(
  SparseStyleMap,
  ConstGetField_InvalidPath
) {
  sparse_style_map        mut_style_map{test_type_info};
  const sparse_style_map& style_map{mut_style_map};

  static cydui::position_e test_value = cydui::position_e::ABSOLUTE;
  bool                     changed =
    mut_style_map.set_field("position", refl::any::make<cydui::position_e>(test_value));
  EXPECT_TRUE(changed);
  EXPECT_FALSE(style_map.empty());
  EXPECT_EQ(style_map.size(), 1);

  std::optional<refl::any> value = style_map.get_field("not_a_field");
  ASSERT_FALSE(value.has_value());
}

TEST(
  SparseStyleMap,
  ErasePath_BaseStyle
) {
  sparse_style_map style_map{test_type_info};

  std::optional<refl::field_path> path_opt = style_map.find_path("position");
  ASSERT_TRUE(path_opt.has_value());
  refl::field_path& path = path_opt.value();

  static cydui::position_e test_value = cydui::position_e::ABSOLUTE;
  bool changed = style_map.set_field(path, refl::any::make<cydui::position_e>(test_value));
  EXPECT_TRUE(changed);
  EXPECT_FALSE(style_map.empty());
  EXPECT_EQ(style_map.size(), 1);

  bool changed2 = style_map.erase_path(path);
  EXPECT_TRUE(changed2);
  EXPECT_TRUE(style_map.empty());
  EXPECT_EQ(style_map.size(), 0);
}

TEST(
  SparseStyleMap,
  ErasePath_CustomStyle
) {
  sparse_style_map style_map{test_type_info};

  std::optional<refl::field_path> path_opt = style_map.find_path("a");
  ASSERT_TRUE(path_opt.has_value());
  refl::field_path& path = path_opt.value();

  static int test_value = 12345;
  bool       changed    = style_map.set_field(path, refl::any::make<int>(test_value));
  EXPECT_TRUE(changed);
  EXPECT_FALSE(style_map.empty());
  EXPECT_EQ(style_map.size(), 1);

  bool changed2 = style_map.erase_path(path);
  EXPECT_TRUE(changed2);
  EXPECT_TRUE(style_map.empty());
  EXPECT_EQ(style_map.size(), 0);
}

TEST(
  SparseStyleMap,
  Clear
) {
  sparse_style_map style_map{test_type_info};

  std::optional<refl::field_path> path_opt = style_map.find_path("a");
  ASSERT_TRUE(path_opt.has_value());
  refl::field_path& path = path_opt.value();

  static int test_value = 12345;
  bool       changed    = style_map.set_field(path, refl::any::make<int>(test_value));
  EXPECT_TRUE(changed);
  EXPECT_FALSE(style_map.empty());
  EXPECT_EQ(style_map.size(), 1);

  style_map.clear();

  EXPECT_TRUE(style_map.empty());
  EXPECT_EQ(style_map.size(), 0);
}

TEST(
  SparseStyleMap,
  ApplyToObject
) {
  sparse_style_map style_map{test_type_info};

  std::optional<refl::field_path> path_opt = style_map.find_path("a");
  ASSERT_TRUE(path_opt.has_value());
  refl::field_path& path = path_opt.value();

  static int test_value = 12345;
  bool       changed    = style_map.set_field(path, refl::any::make<int>(test_value));
  EXPECT_TRUE(changed);
  EXPECT_FALSE(style_map.empty());
  EXPECT_EQ(style_map.size(), 1);

  auto           style_obj_ptr = std::make_shared<test_obj_t>();
  style_object_t style_object{style_obj_ptr};

  EXPECT_EQ(style_obj_ptr->a, 1);
  style_map.apply_to_object(style_object);
  EXPECT_EQ(style_obj_ptr->a, test_value);
}

TEST(
  CustomConversions,
  FromFieldMetadata
) {
  sparse_style_map style_map{test_type_info};

  static int test_value = 12345;
  bool       changed    = style_map.set_field("custom", refl::any::make<int>(test_value));
  EXPECT_TRUE(changed);
  EXPECT_FALSE(style_map.empty());
  EXPECT_EQ(style_map.size(), 1);

  std::optional<refl::any_ref> value = style_map.get_field("custom");
  ASSERT_TRUE(value.has_value());
  EXPECT_EQ(value.value().type().name(), refl::type_name<custom_type>);
  EXPECT_EQ(value.value().as<custom_type>().str, std::format("VAL:{}", test_value));
}

TEST(
  CustomConversions,
  FromConversionRegistry
) {
  sparse_style_map style_map{test_type_info};

  cydui::style::set_custom_type_conversion<int, custom_type2>([](const refl::any& it) {
    return refl::any::make<custom_type2>({.str = std::format("{}->VAL", it.as<int>())});
  });

  static int test_value = 12345;
  bool       changed    = style_map.set_field("custom2", refl::any::make<int>(test_value));
  EXPECT_TRUE(changed);
  EXPECT_FALSE(style_map.empty());
  EXPECT_EQ(style_map.size(), 1);

  std::optional<refl::any_ref> value = style_map.get_field("custom2");
  ASSERT_TRUE(value.has_value());
  EXPECT_EQ(value.value().type().name(), refl::type_name<custom_type2>);
  EXPECT_EQ(value.value().as<custom_type2>().str, std::format("{}->VAL", test_value));
}

TEST(
  CustomConversions,
  MissingConversion
) {
  sparse_style_map style_map{test_type_info};

  static int test_value = 12345;
  bool       changed    = style_map.set_field("custom3", refl::any::make<int>(test_value));
  EXPECT_FALSE(changed);
  EXPECT_TRUE(style_map.empty());
  EXPECT_EQ(style_map.size(), 0);
}

TEST(
  CustomConversions,
  FromList
) {
  sparse_style_map style_map{test_type_info};

  static std::string test_value = "hello world";
  bool               changed    = style_map.set_field(
    "custom3",
    refl::any::make<std::vector<refl::any>>(std::vector{refl::any::make<std::string>(test_value)})
  );
  EXPECT_TRUE(changed);
  EXPECT_FALSE(style_map.empty());
  EXPECT_EQ(style_map.size(), 1);

  std::optional<refl::any_ref> value = style_map.get_field("custom3.str");
  ASSERT_TRUE(value.has_value());
  ASSERT_EQ(value.value().type().name(), refl::type_name<std::string>);
  EXPECT_EQ(value.value().as<std::string>(), test_value);
}

TEST(
  CustomConversions,
  FromList_WrongItems
) {
  sparse_style_map style_map{test_type_info};

  static int test_value = 12345;
  bool       changed    = style_map.set_field(
    "custom3",
    refl::any::make<std::vector<refl::any>>(std::vector{refl::any::make<int>(test_value)})
  );
  EXPECT_FALSE(changed);
  EXPECT_TRUE(style_map.empty());
  EXPECT_EQ(style_map.size(), 0);
}

TEST(
  CustomConversions,
  FromList_TooManyItems
) {
  sparse_style_map style_map{test_type_info};

  static std::string test_value = "hello world";
  bool               changed    = style_map.set_field(
    "custom3",
    refl::any::make<std::vector<refl::any>>(std::vector{
      refl::any::make<std::string>(test_value), refl::any::make<std::string>(test_value)
    })
  );
  EXPECT_FALSE(changed);
  EXPECT_TRUE(style_map.empty());
  EXPECT_EQ(style_map.size(), 0);
}

TEST(
  CustomConversions,
  FromArchive
) {
  sparse_style_map style_map{test_type_info};

  static std::string test_value = "hello world";
  refl::archive      archive{};
  archive["str"] = std::string{test_value};
  bool changed   = style_map.set_field("custom3", refl::any::make<refl::archive>(archive));
  EXPECT_TRUE(changed);
  EXPECT_FALSE(style_map.empty());
  EXPECT_EQ(style_map.size(), 1);

  std::optional<refl::any_ref> value = style_map.get_field("custom3.str");
  ASSERT_TRUE(value.has_value());
  ASSERT_EQ(value.value().type().name(), refl::type_name<std::string>);
  EXPECT_EQ(value.value().as<std::string>(), test_value);
}

TEST(
  CustomConversions,
  FromArchive_UnknownFields
) {
  sparse_style_map style_map{test_type_info};

  static int    test_value = 12345;
  refl::archive archive{};
  archive["a"] = refl::any::make<int>(test_value);
  bool changed = style_map.set_field("custom3", refl::any::make<refl::archive>(archive));
  EXPECT_FALSE(changed);
  EXPECT_TRUE(style_map.empty());
  EXPECT_EQ(style_map.size(), 0);
}
