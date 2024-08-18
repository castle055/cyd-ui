// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

export module cydui.components:dimensions;

import std;

export {
namespace cyd::ui::dimensions {

    typedef int dimension_value_t;
    
    class dimension_t;
    
    struct dimensional_relation_t {
      std::unordered_set<dimension_t*> deps = {};
      std::function<dimension_value_t()> binding = []() {
        return 0;
      };
      
      dimensional_relation_t(
        std::function<dimension_value_t()> binding,
        std::unordered_set<dimension_t*> deps
      );
      
      dimensional_relation_t(// NOLINT(google-explicit-constructor)
        dimension_value_t value
      );
      
      dimensional_relation_t(// NOLINT(google-explicit-constructor)
        dimension_t &dim
      );
      
      operator dimension_value_t() const {
        return this->binding();
      }
      
      dimension_value_t val() const {
        return this->binding();
      }
      
      //dimensional_relation_t(// NOLINT(google-explicit-constructor)
      //  dimensional_relation_t const &rel
      //) = default;
    };
    
    class dimension_t {
      dimension_value_t value = 0;
    
    public:
      static bool compute(dimension_t &dimension);
      
      bool unknown = false;
      std::unordered_set<dimension_t*> deps;
      std::function<dimension_value_t()> binding = []() {
        return 0;
      };
      
      operator dimension_value_t() const;// NOLINT(google-explicit-constructor)
      dimension_value_t val() const;
      
      dimension_t() = default;
      
      dimension_t(dimension_value_t value);// NOLINT(google-explicit-constructor)
      
      ~dimension_t() {
        deps.clear();
      }
      
      /* *
       * Copy constructor comes in handy sometimes, so NOT deleting it
       *
       * HOWEVER, BE AWARE that if you have A depending on B and you copy A to C,
       * B still depends on A and not C.
       */
      //dimension_t(dimension_t &value) = delete;
      
      dimension_t &operator=(dimension_value_t value);
      
      dimension_t &operator=(dimension_t const &dim);
      
      dimension_t &operator=(dimensional_relation_t value);
    };

#define DIMENSION_OPERATOR_H(OP) \
  dimensional_relation_t operator OP( \
      const dimension_t& dim, dimension_t& value); \
  dimensional_relation_t operator OP( \
      const dimension_t& dim, dimension_value_t value); \
  dimensional_relation_t operator OP( \
      const dimension_t& dim, double value); \
  dimensional_relation_t operator OP( \
      dimension_value_t value, dimension_t& dim);  \
  dimensional_relation_t operator OP( \
      const dimension_t& dim, const dimensional_relation_t& value);
    
    
    DIMENSION_OPERATOR_H(+)
    
    DIMENSION_OPERATOR_H(-)
    
    DIMENSION_OPERATOR_H(*)
    
    DIMENSION_OPERATOR_H(/)
    
    // DIMENSION_OPERATOR_H(%)

#undef DIMENSION_OPERATOR_H

#define DIMENSION_REL_OPERATOR_H(OP) \
  dimensional_relation_t operator OP(\
      const dimensional_relation_t r, dimension_t& value); \
  dimensional_relation_t operator OP(\
      const dimensional_relation_t r, dimension_value_t value); \
  dimensional_relation_t operator OP(\
      const dimensional_relation_t r, double value); \
  dimensional_relation_t operator OP(\
      dimension_value_t value, dimensional_relation_t r);  \
  dimensional_relation_t operator OP(\
      const dimensional_relation_t r, const dimensional_relation_t value);
    
    
    DIMENSION_REL_OPERATOR_H(+)
    
    DIMENSION_REL_OPERATOR_H(-)
    
    DIMENSION_REL_OPERATOR_H(*)
    
    DIMENSION_REL_OPERATOR_H(/)
    
    // DIMENSION_REL_OPERATOR_H(%)

#undef DIMENSION_REL_OPERATOR_H
    
    struct four_sided_dimension_t {
      dimension_t top = 0;
      dimension_t right = 0;
      dimension_t bottom = 0;
      dimension_t left = 0;
      
      four_sided_dimension_t &operator=(dimension_value_t val);
    };
    
    struct component_dimensions_t {
      dimension_t x = 0;
      dimension_t y = 0;
      dimension_t w = 0;
      dimension_t h = 0;
      
      bool given_w = false;
      bool given_h = false;
      
      dimension_t cx = 0;
      dimension_t cy = 0;
      dimension_t cw = 0;
      dimension_t ch = 0;
      
      four_sided_dimension_t margin;
      four_sided_dimension_t padding;
    };
}// namespace cyd::ui::dimensions


}

using namespace cyd::ui;

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

dimensions::dimensional_relation_t::dimensional_relation_t(
  dimensions::dimension_t &dim) {
  this->deps = {&dim};
  this->binding = [&dim]() { return dim.val(); };
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
    this->binding = std::move(val.binding);
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
      const dimension_t& dim, double val) {                         \
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

// DIMENSION_OPERATOR(%)

#undef DIMENSION_OPERATOR

#define DIMENSION_REL_OPERATOR(OP)                                             \
  dimensions::dimensional_relation_t dimensions::operator OP(                  \
      const dimensional_relation_t r, dimension_value_t val) {                \
    return dimensional_relation_t(                                                                   \
        [r, val]() { return r.binding() OP val; },                 \
        r.deps                                                     \
    );                                                                         \
  }                                                                            \
                                                                               \
  dimensions::dimensional_relation_t dimensions::operator OP(                  \
      const dimensional_relation_t r, double val) {                \
    return dimensional_relation_t(                                                                   \
        [r, val]() { return r.binding() OP val; },                 \
        r.deps                                                     \
    );                                                                         \
  }                                                                            \
                                                                               \
  dimensions::dimensional_relation_t dimensions::operator OP(                  \
      dimension_value_t val, dimensional_relation_t r) {                      \
    return dimensional_relation_t(                                                                   \
        [r, val]() { return r.binding() OP val; },                 \
        r.deps                                                     \
    );                                                                         \
  }                                                                            \
                                                                               \
  dimensions::dimensional_relation_t dimensions::operator OP(                  \
      const dimensional_relation_t r, dimension_t& dim) {                     \
    dimensional_relation_t rel(                                             \
        [r, &dim]() { return r.binding() OP dim.val(); },          \
        {&dim}                                                     \
    );                                                                         \
    for (const auto& item: r.deps)                                             \
      rel.deps.insert(item);                                                   \
                                                                               \
    return rel;                                                                \
  }                                                                            \
                                                                               \
  dimensions::dimensional_relation_t dimensions::operator OP(                  \
      const dimensional_relation_t r, const dimensional_relation_t val) {    \
    dimensional_relation_t rel(                                             \
        [r, val]() { return r.binding() OP val.binding(); },       \
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

// DIMENSION_REL_OPERATOR(%)

#undef DIMENSION_REL_OPERATOR
