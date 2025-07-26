//
// Created by castle on 8/15/24.
//

#include "gtest/gtest.h"

import cydui.styling.sparse_field_map;
import cydui.core.focus_state;

using namespace cydui::style;

struct inner_test_obj_t {
  int c = 5;
};

struct test_obj_t {
  int              a = 1;
  double           b = 2.0;
  std::string      s = "hello world";
  inner_test_obj_t o{};
};

const refl::type_info& test_type_info = refl::type_info::from<test_obj_t>();

TEST(
  SparseFieldMap,
  Constructor
) {
  sparse_field_map field_map{test_type_info};
  EXPECT_EQ(field_map.size(), 0);
  EXPECT_EQ(field_map.get_type().id(), test_type_info.id());
  EXPECT_TRUE(field_map.empty());
}

TEST(
  SparseFieldMap,
  FindPath
) {
  sparse_field_map field_map{test_type_info};

  std::optional<refl::field_path> result_a = field_map.find_path("a");
  std::optional<refl::field_path> result_b = field_map.find_path("b");
  std::optional<refl::field_path> result_s = field_map.find_path("s");
  std::optional<refl::field_path> result_o = field_map.find_path("o");

  ASSERT_TRUE(result_a.has_value());
  EXPECT_EQ(result_a.value().type().id(), refl::type_id<int>);

  ASSERT_TRUE(result_b.has_value());
  EXPECT_EQ(result_b.value().type().id(), refl::type_id<double>);

  ASSERT_TRUE(result_s.has_value());
  EXPECT_EQ(result_s.value().type().id(), refl::type_id<std::string>);

  ASSERT_TRUE(result_o.has_value());
  EXPECT_EQ(result_o.value().type().id(), refl::type_id<inner_test_obj_t>);
}

TEST(
  SparseFieldMap,
  FindPath_NestedPath
) {
  sparse_field_map field_map{test_type_info};

  std::optional<refl::field_path> result_o = field_map.find_path("o");
  std::optional<refl::field_path> result_c = field_map.find_path("o.c");

  ASSERT_TRUE(result_o.has_value());
  EXPECT_EQ(result_o.value().type().id(), refl::type_id<inner_test_obj_t>);

  ASSERT_TRUE(result_c.has_value());
  EXPECT_EQ(result_c.value().type().id(), refl::type_id<int>);
}

TEST(
  SparseFieldMap,
  FindPath_NotFound
) {
  sparse_field_map                field_map{test_type_info};
  std::optional<refl::field_path> result = field_map.find_path("not_a_field");
  ASSERT_FALSE(result.has_value());
}

TEST(
  SparseFieldMap,
  Set
) {
  sparse_field_map                field_map{test_type_info};
  // Find field path
  std::optional<refl::field_path> result = field_map.find_path("a");
  ASSERT_TRUE(result.has_value());
  const refl::field_path& path = *result;

  // Set value
  const int  test_value = 12345;
  const bool changed    = field_map.set(path, test_value);
  EXPECT_TRUE(changed);

  // Assert map now contains value for path
  const bool contains_val          = field_map.contains_value(path);
  const bool contains_specific_val = field_map.contains_specific_value(path);

  ASSERT_TRUE(contains_val);
  ASSERT_TRUE(contains_specific_val);

  // Check value was correctly stored
  std::optional<refl::any_ref> value = field_map.get(path);
  const refl::any_ref&         ref   = *value;
  ASSERT_EQ(ref.type().id(), refl::type_id<int>);
  EXPECT_EQ(ref.as<int>(), test_value);
}

TEST(
  SparseFieldMap,
  Set_NestedPath
) {
  sparse_field_map                field_map{test_type_info};
  // Find field path
  std::optional<refl::field_path> result_o = field_map.find_path("o");
  std::optional<refl::field_path> result   = field_map.find_path("o.c");
  ASSERT_TRUE(result_o.has_value());
  ASSERT_TRUE(result.has_value());
  const refl::field_path& path_o = *result_o;
  const refl::field_path& path   = *result;

  // Set value
  const int  test_value = 12345;
  const bool changed    = field_map.set(path, test_value);
  EXPECT_TRUE(changed);

  // Assert map now contains value for path
  const bool contains_val_o          = field_map.contains_value(path_o);
  const bool contains_specific_val_o = field_map.contains_specific_value(path_o);

  ASSERT_FALSE(contains_val_o);
  ASSERT_FALSE(contains_specific_val_o);

  const bool contains_val          = field_map.contains_value(path);
  const bool contains_specific_val = field_map.contains_specific_value(path);

  ASSERT_TRUE(contains_val);
  ASSERT_TRUE(contains_specific_val);

  // Check value was correctly stored
  std::optional<refl::any_ref> value = field_map.get(path);
  const refl::any_ref&         ref   = *value;
  ASSERT_EQ(ref.type().id(), refl::type_id<int>);
  EXPECT_EQ(ref.as<int>(), test_value);
}

TEST(
  SparseFieldMap,
  Set_ObjectField
) {
  sparse_field_map                field_map{test_type_info};
  // Find field path
  std::optional<refl::field_path> result = field_map.find_path("o");
  ASSERT_TRUE(result.has_value());
  const refl::field_path& path = *result;

  // Set value
  const inner_test_obj_t test_value{12345};
  const bool             changed = field_map.set(path, test_value);
  EXPECT_TRUE(changed);

  // Assert map now contains value for path
  const bool contains_val          = field_map.contains_value(path);
  const bool contains_specific_val = field_map.contains_specific_value(path);

  ASSERT_TRUE(contains_val);
  ASSERT_TRUE(contains_specific_val);

  // Check value was correctly stored
  std::optional<refl::any_ref> value = field_map.get(path);
  const refl::any_ref&         ref   = *value;
  ASSERT_EQ(ref.type().id(), refl::type_id<inner_test_obj_t>);
  EXPECT_EQ(ref.as<inner_test_obj_t>().c, test_value.c);
}

TEST(
  SparseFieldMap,
  Set_DoubleSet
) {
  sparse_field_map                field_map{test_type_info};
  // Find field path
  std::optional<refl::field_path> result = field_map.find_path("a");
  ASSERT_TRUE(result.has_value());
  const refl::field_path& path = *result;

  // Set value
  const int test_value = 12345;
  bool      changed    = field_map.set(path, test_value);
  EXPECT_TRUE(changed);

  // Set value AGAIN
  changed = field_map.set(path, test_value);
  EXPECT_FALSE(changed);

  // Assert map now contains value for path
  const bool contains_val          = field_map.contains_value(path);
  const bool contains_specific_val = field_map.contains_specific_value(path);

  ASSERT_TRUE(contains_val);
  ASSERT_TRUE(contains_specific_val);

  // Check value was correctly stored
  std::optional<refl::any_ref> value = field_map.get(path);
  const refl::any_ref&         ref   = *value;
  ASSERT_EQ(ref.type().id(), refl::type_id<int>);
  EXPECT_EQ(ref.as<int>(), test_value);
}

TEST(
  SparseFieldMap,
  Set_OverridePreviousValue
) {
  sparse_field_map                field_map{test_type_info};
  // Find field path
  std::optional<refl::field_path> result = field_map.find_path("a");
  ASSERT_TRUE(result.has_value());
  const refl::field_path& path = *result;

  // Set value
  const int test_value = 12345;
  bool      changed    = field_map.set(path, test_value);
  EXPECT_TRUE(changed);

  // Set value AGAIN
  const int new_test_value = 54321;
  changed                  = field_map.set(path, new_test_value);
  EXPECT_TRUE(changed);

  // Check map size is correct
  ASSERT_EQ(field_map.size(), 1);

  // Assert map now contains value for path
  const bool contains_val          = field_map.contains_value(path);
  const bool contains_specific_val = field_map.contains_specific_value(path);

  ASSERT_TRUE(contains_val);
  ASSERT_TRUE(contains_specific_val);

  // Check value was correctly stored
  std::optional<refl::any_ref> value = field_map.get(path);
  const refl::any_ref&         ref   = *value;
  ASSERT_EQ(ref.type().id(), refl::type_id<int>);
  EXPECT_EQ(ref.as<int>(), new_test_value);
}

TEST(
  SparseFieldMap,
  Set_InvalidPath
) {
  sparse_field_map field_map{test_type_info};
  // Create field path
  struct other_test_obj_t {
    int a;
  };
  const refl::field_path path =
    refl::field_path::from_string(refl::type_info::from<other_test_obj_t>(), "a").value();

  // Set value
  const int  test_value = 12345;
  const bool changed    = field_map.set(path, test_value);
  EXPECT_FALSE(changed);

  // Check map still does not contain value for path
  const bool contains_val          = field_map.contains_value(path);
  const bool contains_specific_val = field_map.contains_specific_value(path);

  EXPECT_FALSE(contains_val);
  EXPECT_FALSE(contains_specific_val);
}

TEST(
  SparseFieldMap,
  Erase
) {
  sparse_field_map                field_map{test_type_info};
  // Find field path
  std::optional<refl::field_path> result = field_map.find_path("a");
  ASSERT_TRUE(result.has_value());
  const refl::field_path& path = *result;

  // Set value
  const int  test_value = 12345;
  const bool changed    = field_map.set(path, test_value);
  EXPECT_TRUE(changed);

  // Check map now contains value for path
  bool contains_val          = field_map.contains_value(path);
  bool contains_specific_val = field_map.contains_specific_value(path);

  EXPECT_TRUE(contains_val);
  EXPECT_TRUE(contains_specific_val);

  // Erase value for path
  const bool erased = field_map.erase(path);
  EXPECT_TRUE(erased);

  // Check map now DOES NOT contain value for path
  contains_val          = field_map.contains_value(path);
  contains_specific_val = field_map.contains_specific_value(path);

  EXPECT_FALSE(contains_val);
  EXPECT_FALSE(contains_specific_val);
}

TEST(
  SparseFieldMap,
  Erase_NotPresent
) {
  sparse_field_map                field_map{test_type_info};
  // Find field path
  std::optional<refl::field_path> result = field_map.find_path("a");
  ASSERT_TRUE(result.has_value());
  const refl::field_path& path = *result;

  // Check map is empty
  EXPECT_TRUE(field_map.empty());

  // Erase value for path
  const bool erased = field_map.erase(path);
  EXPECT_FALSE(erased);

  // Check map still empty
  EXPECT_TRUE(field_map.empty());
}

TEST(
  SparseFieldMap,
  Erase_DoubleErase
) {
  sparse_field_map                field_map{test_type_info};
  // Find field path
  std::optional<refl::field_path> result = field_map.find_path("a");
  ASSERT_TRUE(result.has_value());
  const refl::field_path& path = *result;

  // Set value
  const int  test_value = 12345;
  const bool changed    = field_map.set(path, test_value);
  EXPECT_TRUE(changed);

  // Check map now contains value for path
  bool contains_val          = field_map.contains_value(path);
  bool contains_specific_val = field_map.contains_specific_value(path);

  EXPECT_TRUE(contains_val);
  EXPECT_TRUE(contains_specific_val);

  // Erase value for path
  bool erased = field_map.erase(path);
  EXPECT_TRUE(erased);

  // Erase value for path AGAIN
  erased = field_map.erase(path);
  EXPECT_FALSE(erased);

  // Check map now DOES NOT contain value for path
  contains_val          = field_map.contains_value(path);
  contains_specific_val = field_map.contains_specific_value(path);

  EXPECT_FALSE(contains_val);
  EXPECT_FALSE(contains_specific_val);
}

TEST(
  SparseFieldMap,
  Erase_InvalidPath
) {
  sparse_field_map                field_map{test_type_info};
  // Find field path
  std::optional<refl::field_path> result = field_map.find_path("a");
  ASSERT_TRUE(result.has_value());
  const refl::field_path& real_path = *result;

  // Set value
  const int  test_value = 12345;
  const bool changed    = field_map.set(real_path, test_value);
  EXPECT_TRUE(changed);
  ASSERT_EQ(field_map.size(), 1);

  // Create field path
  struct other_test_obj_t {
    int a;
  };
  const refl::field_path path =
    refl::field_path::from_string(refl::type_info::from<other_test_obj_t>(), "a").value();

  // Erase value for path
  const bool erased = field_map.erase(path);
  EXPECT_FALSE(erased);

  EXPECT_EQ(field_map.size(), 1);
}

TEST(
  SparseFieldMap,
  Get
) {
  sparse_field_map                field_map{test_type_info};
  // Find field path
  std::optional<refl::field_path> result = field_map.find_path("a");
  ASSERT_TRUE(result.has_value());
  const refl::field_path& path = *result;

  // Set value
  const int  test_value = 12345;
  const bool changed    = field_map.set(path, test_value);
  EXPECT_TRUE(changed);
  ASSERT_EQ(field_map.size(), 1);

  // Get value for path
  std::optional<refl::any_ref> value = field_map.get(path);
  EXPECT_TRUE(value.has_value());
  const refl::any_ref& ref = *value;
  ASSERT_EQ(ref.type().id(), refl::type_id<int>);
  EXPECT_EQ(ref.as<int>(), test_value);
}

TEST(
  SparseFieldMap,
  Get_NestedField
) {
  sparse_field_map                field_map{test_type_info};
  // Find field path
  std::optional<refl::field_path> result_o = field_map.find_path("o");
  std::optional<refl::field_path> result   = field_map.find_path("o.c");
  ASSERT_TRUE(result_o.has_value());
  ASSERT_TRUE(result.has_value());
  const refl::field_path& path_o = *result_o;
  const refl::field_path& path   = *result;

  // Set value
  const inner_test_obj_t test_value{12345};
  const bool             changed = field_map.set(path_o, test_value);
  EXPECT_TRUE(changed);

  // Assert map now contains value for path
  const bool contains_val_o          = field_map.contains_value(path_o);
  const bool contains_specific_val_o = field_map.contains_specific_value(path_o);

  ASSERT_TRUE(contains_val_o);
  ASSERT_TRUE(contains_specific_val_o);

  const bool contains_val          = field_map.contains_value(path);
  const bool contains_specific_val = field_map.contains_specific_value(path);

  ASSERT_TRUE(contains_val);
  ASSERT_FALSE(contains_specific_val);

  // Check value was correctly stored
  std::optional<refl::any_ref> value = field_map.get(path);
  const refl::any_ref&         ref   = *value;
  ASSERT_EQ(ref.type().id(), refl::type_id<int>);
  EXPECT_EQ(ref.as<int>(), test_value.c);
}

TEST(
  SparseFieldMap,
  Get_NotFound
) {
  sparse_field_map                field_map{test_type_info};
  // Find field path
  std::optional<refl::field_path> result = field_map.find_path("a");
  ASSERT_TRUE(result.has_value());
  const refl::field_path& path = *result;

  // Get value for path
  std::optional<refl::any_ref> value = field_map.get(path);
  EXPECT_FALSE(value.has_value());
}

TEST(
  SparseFieldMap,
  Get_InvalidPath
) {
  sparse_field_map                field_map{test_type_info};
  // Find field path
  std::optional<refl::field_path> result = field_map.find_path("a");
  ASSERT_TRUE(result.has_value());
  const refl::field_path& real_path = *result;

  // Set value
  const int  test_value = 12345;
  const bool changed    = field_map.set(real_path, test_value);
  EXPECT_TRUE(changed);
  ASSERT_EQ(field_map.size(), 1);

  // Create field path
  struct other_test_obj_t {
    int a;
  };
  const refl::field_path path =
    refl::field_path::from_string(refl::type_info::from<other_test_obj_t>(), "a").value();

  // Get value for path
  std::optional<refl::any_ref> value = field_map.get(path);
  EXPECT_FALSE(value.has_value());
}

TEST(
  SparseFieldMap,
  ConstGet
) {
  sparse_field_map                mut_field_map{test_type_info};
  const sparse_field_map&         field_map = mut_field_map;
  // Find field path
  std::optional<refl::field_path> result    = field_map.find_path("a");
  ASSERT_TRUE(result.has_value());
  const refl::field_path& path = *result;

  // Set value
  const int  test_value = 12345;
  const bool changed    = mut_field_map.set(path, test_value);
  EXPECT_TRUE(changed);
  ASSERT_EQ(field_map.size(), 1);

  // Get value for path
  std::optional<refl::any> value = field_map.get(path);
  EXPECT_TRUE(value.has_value());
  const refl::any& ref = *value;
  ASSERT_EQ(ref.type().id(), refl::type_id<int>);
  EXPECT_EQ(ref.as<int>(), test_value);
}

TEST(
  SparseFieldMap,
  ConstGet_NestedField
) {
  sparse_field_map                mut_field_map{test_type_info};
  const sparse_field_map&         field_map = mut_field_map;
  // Find field path
  std::optional<refl::field_path> result_o  = field_map.find_path("o");
  std::optional<refl::field_path> result    = field_map.find_path("o.c");
  ASSERT_TRUE(result_o.has_value());
  ASSERT_TRUE(result.has_value());
  const refl::field_path& path_o = *result_o;
  const refl::field_path& path   = *result;

  // Set value
  const inner_test_obj_t test_value{12345};
  const bool             changed = mut_field_map.set(path_o, test_value);
  EXPECT_TRUE(changed);

  // Assert map now contains value for path
  const bool contains_val_o          = field_map.contains_value(path_o);
  const bool contains_specific_val_o = field_map.contains_specific_value(path_o);

  ASSERT_TRUE(contains_val_o);
  ASSERT_TRUE(contains_specific_val_o);

  const bool contains_val          = field_map.contains_value(path);
  const bool contains_specific_val = field_map.contains_specific_value(path);

  ASSERT_TRUE(contains_val);
  ASSERT_FALSE(contains_specific_val);

  // Check value was correctly stored
  std::optional<refl::any> value = field_map.get(path);
  const refl::any&         ref   = *value;
  ASSERT_EQ(ref.type().id(), refl::type_id<int>);
  EXPECT_EQ(ref.as<int>(), test_value.c);
}

TEST(
  SparseFieldMap,
  ConstGet_NotFound
) {
  sparse_field_map                mut_field_map{test_type_info};
  const sparse_field_map&         field_map = mut_field_map;
  // Find field path
  std::optional<refl::field_path> result    = field_map.find_path("a");
  ASSERT_TRUE(result.has_value());
  const refl::field_path& path = *result;

  // Get value for path
  std::optional<refl::any> value = field_map.get(path);
  EXPECT_FALSE(value.has_value());
}

TEST(
  SparseFieldMap,
  ConstGet_InvalidPath
) {
  sparse_field_map                mut_field_map{test_type_info};
  const sparse_field_map&         field_map = mut_field_map;
  // Find field path
  std::optional<refl::field_path> result    = field_map.find_path("a");
  ASSERT_TRUE(result.has_value());
  const refl::field_path& real_path = *result;

  // Set value
  const int  test_value = 12345;
  const bool changed    = mut_field_map.set(real_path, test_value);
  EXPECT_TRUE(changed);
  ASSERT_EQ(field_map.size(), 1);

  // Create field path
  struct other_test_obj_t {
    int a;
  };
  const refl::field_path path =
    refl::field_path::from_string(refl::type_info::from<other_test_obj_t>(), "a").value();

  // Get value for path
  std::optional<refl::any> value = field_map.get(path);
  EXPECT_FALSE(value.has_value());
}

TEST(
  SparseFieldMap,
  Clear
) {
  sparse_field_map                field_map{test_type_info};
  // Find field path
  std::optional<refl::field_path> result = field_map.find_path("a");
  ASSERT_TRUE(result.has_value());
  const refl::field_path& real_path = *result;

  // Set value
  const int  test_value = 12345;
  const bool changed    = field_map.set(real_path, test_value);
  EXPECT_TRUE(changed);
  ASSERT_EQ(field_map.size(), 1);

  // Clear
  field_map.clear();

  EXPECT_TRUE(field_map.empty());
  EXPECT_EQ(field_map.size(), 0);
}
