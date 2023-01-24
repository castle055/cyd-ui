//
// Created by castle on 1/23/23.
//

#include "dimensions.hpp"

#include <utility>

using namespace cydui;

bool dimensions::dimension_t::compute(dimensions::dimension_t& dimension) {
  // Compute value for all dependencies recursively, catch circular deps
  dimension.unknown = true;
  for (auto& item: dimension.deps) {
    if (item->unknown || !compute(*item)) {
      return false;
    }
  }

  // Compute own value
  dimension.value   = dimension.binding();
  dimension.unknown = false;
  return true;
}

// OPERATORS, CONSTRUCTORS AND UTILS
dimensions::dimensional_relation_t::dimensional_relation_t(
        std::function<dimension_value_t()> _binding,
        std::unordered_set<dimension_t*> _deps
): deps(std::move(_deps)), binding(std::move(_binding)) { }

dimensions::dimensional_relation_t::dimensional_relation_t(
    dimensions::dimension_value_t val) {
  this->binding = [val]() { return val; };
}

dimensions::dimension_t::dimension_t(dimensions::dimension_value_t val) {
  this->value   = val;
  this->binding = [val]() { return val; };
  this->unknown = false;
}

//dimensions::dimension_t::dimension_t(dimensions::dimension_t &value) {
//  this->deps.push_back(&value);
//  this->binding = [this]() { return *(this->deps[0]); };
//  this->unknown = false;
//}

dimensions::dimension_t::operator dimension_value_t() const {
  return value;
}

dimensions::dimension_value_t dimensions::dimension_t::val() const {
  return value;
}

dimensions::dimension_t& dimensions::dimension_t::operator=(
    dimension_value_t val) {
  this->deps.clear();
  this->value   = val;
  this->binding = [val]() { return val; };
    this->unknown = false;
    return *this;
}

dimensions::dimension_t& dimensions::dimension_t::operator=(
        dimensions::dimension_t const& dim) {
    if (this == &dim) {
        return *this;// Is this really needed??
    }
    this->deps.clear();
    this->deps.insert((dimension_t*)&dim);
    this->binding = [&dim]() { return dim; };
    this->unknown = false;

    return *this;
}

dimensions::dimension_t& dimensions::dimension_t::operator=(
    dimensional_relation_t val) {
    this->deps.clear();
    this->deps    = val.deps;
    this->binding = [val]() { return val.binding(); };
    this->unknown = false;
    return *this;
}

dimensions::four_sided_dimension_t &dimensions::four_sided_dimension_t::operator=(dimension_value_t val) {
    this->top = val;
    this->right = val;
    this->bottom = val;
    this->left = val;
    return *this;
}

#define DIMENSION_OPERATOR(OP)                                                 \
  dimensions::dimensional_relation_t dimensions::operator OP(                  \
      const dimension_t& dim, dimension_value_t val) {                         \
    return dimensional_relation_t(                                                                   \
        [&dim, val]() { return dim.val() OP val; },                 \
        {(dimension_t*)&dim}                                       \
    );                                                                         \
  }                                                                            \
                                                                               \
  dimensions::dimensional_relation_t dimensions::operator OP(                  \
      dimension_value_t val, dimension_t& dim) {                               \
    return dimensional_relation_t(                                                                   \
        [&dim, val]() { return dim.val() OP val; },                 \
        {&dim}                                                     \
    );                                                                         \
  }                                                                            \
                                                                               \
  dimensions::dimensional_relation_t dimensions::operator OP(                  \
      const dimension_t& dim, dimension_t& dim1) {                             \
    return dimensional_relation_t(                                                                   \
        [&dim, &dim1]() { return dim.val() OP dim1.val(); },        \
        {(dimension_t*)&dim, (dimension_t*)&dim1}                  \
    );                                                                         \
  }                                                                            \
                                                                               \
  dimensions::dimensional_relation_t dimensions::operator OP(                  \
      const dimension_t& dim, const dimensional_relation_t& val) {             \
    dimensional_relation_t rel(                                             \
        [&dim, val]() { return dim.val() OP val.binding(); },       \
        {(dimension_t*)&dim}                                       \
    );                                                                         \
    for (const auto& item: val.deps)                                           \
      rel.deps.insert(item);                                                   \
                                                                               \
    return rel;                                                                \
  }

DIMENSION_OPERATOR(+)

DIMENSION_OPERATOR(-)

DIMENSION_OPERATOR(*)

DIMENSION_OPERATOR(/)

DIMENSION_OPERATOR(%)

#undef DIMENSION_OPERATOR

#define DIMENSION_REL_OPERATOR(OP)                                             \
  dimensions::dimensional_relation_t dimensions::operator OP(                  \
      const dimensional_relation_t& r, dimension_value_t val) {                \
    return dimensional_relation_t(                                                                   \
        [&r, val]() { return r.binding() OP val; },                 \
        r.deps                                                     \
    );                                                                         \
  }                                                                            \
                                                                               \
  dimensions::dimensional_relation_t dimensions::operator OP(                  \
      dimension_value_t val, dimensional_relation_t& r) {                      \
    return dimensional_relation_t(                                                                   \
        [&r, val]() { return r.binding() OP val; },                 \
        r.deps                                                     \
    );                                                                         \
  }                                                                            \
                                                                               \
  dimensions::dimensional_relation_t dimensions::operator OP(                  \
      const dimensional_relation_t& r, dimension_t& dim) {                     \
    dimensional_relation_t rel(                                             \
        [&r, &dim]() { return r.binding() OP dim.val(); },          \
        {&dim}                                                     \
    );                                                                         \
    for (const auto& item: r.deps)                                             \
      rel.deps.insert(item);                                                   \
                                                                               \
    return rel;                                                                \
  }                                                                            \
                                                                               \
  dimensions::dimensional_relation_t dimensions::operator OP(                  \
      const dimensional_relation_t& r, const dimensional_relation_t& val) {    \
    dimensional_relation_t rel(                                             \
        [&r, val]() { return r.binding() OP val.binding(); },       \
        {}                                                         \
    );                                                                         \
    for (const auto& item: r.deps)                                             \
      rel.deps.insert(item);                                                   \
                                                                               \
    for (const auto& item: val.deps)                                           \
      rel.deps.insert(item);                                                   \
                                                                               \
    return rel;                                                                \
  }

DIMENSION_REL_OPERATOR(+)

DIMENSION_REL_OPERATOR(-)

DIMENSION_REL_OPERATOR(*)

DIMENSION_REL_OPERATOR(/)

DIMENSION_REL_OPERATOR(%)

#undef DIMENSION_REL_OPERATOR
