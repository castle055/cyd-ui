// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CYD_UI_TESTS_STRUCTURE_H
#define CYD_UI_TESTS_STRUCTURE_H

#include <source_location>
#include <unordered_map>
#include <string>
#include <functional>
#include <utility>
#include <iostream>
#include <cassert>

class test_case_t;

class test_case_list_t {
public:
  std::unordered_map<std::string, test_case_t &> test_cases { };
};

test_case_list_t CASES { };

class test_case_t {
public:
  std::string name;
  std::source_location location;
  std::function<int()> body = []() { return 1; };

  test_case_t(
    const std::string &name_,
    const std::source_location location_,
    std::function<int()> body_
  )
    : name(name_), location(location_), body(std::move(body_)) {
    CASES.test_cases.emplace(std::pair<std::string, test_case_t &> {name_, *this});
  }
};

#define TEST_ID case_
#define TEST_BLOCK_ID case_block_
#define TEST_ID_NUM __LINE__
#define CONCAT_IMPL(A, B) A##B
#define CONCAT(A, B) CONCAT_IMPL(A, B)

#define __TEST_BODY_DECL__(...) []() { \
  __VA_ARGS__                          \
}};

#define __TEST_DECL(NAME, ID) \
int CONCAT(TEST_BLOCK_ID,ID)(); \
test_case_t CONCAT(TEST_ID, ID) { \
  NAME, \
  std::source_location::current(), \
  []() { return CONCAT(TEST_BLOCK_ID,ID)(); } \
}; \
int CONCAT(TEST_BLOCK_ID,ID)()


#define TEST(NAME) __TEST_DECL(NAME, TEST_ID_NUM)

void setup();

int main(int argc, char* argv[]) {
  if (argc != 2) return 1;
  std::string name {argv[1]};

  const auto &test_case = CASES.test_cases.at(name);
  std::cout
    << "Test location: "
    << test_case.location.file_name()
    << ":"
    << test_case.location.line()
    << std::endl;
  std::cout << "Running setup function" << std::endl;
  setup();
  return test_case.body();
}

#endif //CYD_UI_TESTS_STRUCTURE_H
