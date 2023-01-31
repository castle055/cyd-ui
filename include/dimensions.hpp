//
// Created by castle on 8/21/22.
//

#ifndef CYD_UI_DIMENSIONS_HPP
#define CYD_UI_DIMENSIONS_HPP

#include <functional>
#include <unordered_set>
#include <vector>

namespace cydui::dimensions {
    
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
      
      dimensional_relation_t(// NOLINT(google-explicit-constructor)
        dimensional_relation_t const &rel
      ) = default;
    };
    
    class dimension_t {
      dimension_value_t value = 0;
    
    public:
      static bool compute(dimension_t &dimension);
      
      bool unknown = false;
      std::unordered_set<dimension_t*> deps = {};
      std::function<dimension_value_t()> binding = []() {
        return 0;
      };
      
      operator dimension_value_t() const;// NOLINT(google-explicit-constructor)
      dimension_value_t val() const;
      
      dimension_t() = default;
      
      dimension_t(dimension_value_t value);// NOLINT(google-explicit-constructor)
      
      dimension_t(dimension_t &value) = delete;
      
      dimension_t &operator=(dimension_value_t value);
      
      dimension_t &operator=(dimension_t const &dim);
      
      dimension_t &operator=(dimensional_relation_t value);
    };

#define DIMENSION_OPERATOR_H(OP)                                               \
  dimensional_relation_t operator OP(                                          \
      const dimension_t& dim, dimension_t& value);                             \
  dimensional_relation_t operator OP(                                          \
      const dimension_t& dim, dimension_value_t value);                        \
  dimensional_relation_t operator OP(                                          \
      dimension_value_t value, dimension_t& dim);                              \
  dimensional_relation_t operator OP(                                          \
      const dimension_t& dim, const dimensional_relation_t& value);
    
    
    DIMENSION_OPERATOR_H(+)
    
    DIMENSION_OPERATOR_H(-)
    
    DIMENSION_OPERATOR_H(*)
    
    DIMENSION_OPERATOR_H(/)
    
    DIMENSION_OPERATOR_H(%)

#undef DIMENSION_OPERATOR_H

#define DIMENSION_REL_OPERATOR_H(OP)                                           \
  dimensional_relation_t operator OP(                                          \
      const dimensional_relation_t r, dimension_t& value);                    \
  dimensional_relation_t operator OP(                                          \
      const dimensional_relation_t r, dimension_value_t value);               \
  dimensional_relation_t operator OP(                                          \
      dimension_value_t value, dimensional_relation_t r);                     \
  dimensional_relation_t operator OP(                                          \
      const dimensional_relation_t r, const dimensional_relation_t value);
    
    
    DIMENSION_REL_OPERATOR_H(+)
    
    DIMENSION_REL_OPERATOR_H(-)
    
    DIMENSION_REL_OPERATOR_H(*)
    
    DIMENSION_REL_OPERATOR_H(/)
    
    DIMENSION_REL_OPERATOR_H(%)

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
}// namespace cydui::dimensions


#endif//CYD_UI_DIMENSIONS_HPP
