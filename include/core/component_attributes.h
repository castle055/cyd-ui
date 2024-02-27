//
// Created by castle on 10/26/23.
//

#ifndef CYD_UI_COMPONENT_ATTRIBUTES_H
#define CYD_UI_COMPONENT_ATTRIBUTES_H

#include "dimensions.hpp"

struct attribute_i {
  virtual ~attribute_i() = default;
};

#define COMPONENT_ATTRIBUTE(TYPE, NAME, DEFAULT) \
    template<typename E>                         \
    struct attr_##NAME: public attribute_i {     \
      TYPE _##NAME = DEFAULT;                    \
      ~attr_##NAME() override = default;         \
      template<typename S = E, typename = std::enable_if_t<!std::is_void_v<S>>> \
      inline S& NAME(auto& _##NAME##_) {   \
        this->_##NAME = _##NAME##_;              \
        return *(E*)this;                        \
      }                                          \
      template<typename S = E, typename = std::enable_if_t<!std::is_void_v<S>>> \
      inline S& NAME(auto&& _##NAME##_) {  \
        this->_##NAME = _##NAME##_;              \
        return *(E*)this;                        \
      }                                          \
      template<typename S = E, typename = std::enable_if_t<std::is_void_v<S>>> \
      inline void NAME(auto& _##NAME##_) {   \
        this->_##NAME = _##NAME##_;              \
      }                                          \
      template<typename S = E, typename = std::enable_if_t<std::is_void_v<S>>> \
      inline void NAME(auto&& _##NAME##_) {  \
        this->_##NAME = _##NAME##_;              \
      }                                          \
    }

#define COMPONENT_ATTRIBUTE_W_MONITOR(TYPE, NAME, DEFAULT) \
    template<typename E>                                   \
    struct attr_##NAME: public attribute_i {               \
      TYPE _##NAME = DEFAULT;                              \
      bool _##NAME##_has_changed = false;                  \
      ~attr_##NAME() override = default;                   \
      template<typename S = E, typename = std::enable_if_t<!std::is_void_v<S>>> \
      inline S& NAME(auto& _##NAME##_) {                   \
        this->_##NAME = _##NAME##_;                        \
        this->_##NAME##_has_changed = true;                \
        return *(E*)this;                                  \
      }                                                    \
      template<typename S = E, typename = std::enable_if_t<!std::is_void_v<S>>> \
      inline S& NAME(auto&& _##NAME##_) {             \
        this->_##NAME = _##NAME##_;                        \
        this->_##NAME##_has_changed = true;                \
        return *(E*)this;                                  \
      }                                                    \
      template<typename S = E, typename = std::enable_if_t<std::is_void_v<S>>> \
      inline void NAME(auto& _##NAME##_) {                   \
        this->_##NAME = _##NAME##_;                        \
        this->_##NAME##_has_changed = true;                \
      }                                                    \
      template<typename S = E, typename = std::enable_if_t<std::is_void_v<S>>> \
      inline void NAME(auto&& _##NAME##_) {             \
        this->_##NAME = _##NAME##_;                        \
        this->_##NAME##_has_changed = true;                \
      }                                                    \
    }

COMPONENT_ATTRIBUTE(cydui::dimensions::dimension_t, x, 0);
COMPONENT_ATTRIBUTE(cydui::dimensions::dimension_t, y, 0);
COMPONENT_ATTRIBUTE_W_MONITOR(cydui::dimensions::dimension_t, w, 0);
COMPONENT_ATTRIBUTE_W_MONITOR(cydui::dimensions::dimension_t, h, 0);

COMPONENT_ATTRIBUTE(cydui::dimensions::dimension_t, margin_top, 0);
COMPONENT_ATTRIBUTE(cydui::dimensions::dimension_t, margin_bottom, 0);
COMPONENT_ATTRIBUTE(cydui::dimensions::dimension_t, margin_left, 0);
COMPONENT_ATTRIBUTE(cydui::dimensions::dimension_t, margin_right, 0);

COMPONENT_ATTRIBUTE(cydui::dimensions::dimension_t, padding_top, 0);
COMPONENT_ATTRIBUTE(cydui::dimensions::dimension_t, padding_bottom, 0);
COMPONENT_ATTRIBUTE(cydui::dimensions::dimension_t, padding_left, 0);
COMPONENT_ATTRIBUTE(cydui::dimensions::dimension_t, padding_right, 0);

using content = std::vector<cydui::components::component_builder_t>;

template<typename E>
struct attr_content: public attribute_i {
  template<typename S = E, typename = std::enable_if_t<!std::is_void_v<S>>>
  inline S &operator()(content &&_content_) {
    this->_content = _content_;
    return *(E*) this;
  }
  template<typename S = E, typename = std::enable_if_t<!std::is_void_v<S>>>
  inline S &operator()(content &_content_) {
    this->_content = _content_;
    return *(E*) this;
  }
  template<typename S = E, typename = std::enable_if_t<std::is_void_v<S>>>
  inline void operator()(content &&_content_) {
    this->_content = _content_;
  }
  template<typename S = E, typename = std::enable_if_t<std::is_void_v<S>>>
  inline void operator()(content &_content_) {
    this->_content = _content_;
  }
  content _content = {};
};

template<typename T>
struct attrs_margin
  : attr_margin_top<T>,
    attr_margin_bottom<T>,
    attr_margin_left<T>,
    attr_margin_right<T> {
};

template<typename T>
struct attrs_padding
  : attr_padding_top<T>,
    attr_padding_bottom<T>,
    attr_padding_left<T>,
    attr_padding_right<T> {
};

template<typename T>
struct attrs_dimensions
  : attr_x<T>,
    attr_y<T>,
    attr_w<T>,
    attr_h<T> {
};

template<typename T = void>
struct attrs_component
  : attrs_dimensions<T>,
    attrs_margin<T>,
    attrs_padding<T>,
    attr_content<T> {
};
//template<>
//struct attrs_component<void>
//  : attrs_dimensions<void>,
//    attrs_margin<void>,
//    attrs_padding<void>,
//    attr_content<void> {
//};

#undef COMPONENT_ATTRIBUTE
#undef COMPONENT_ATTRIBUTE_W_MONITOR

#endif //CYD_UI_COMPONENT_ATTRIBUTES_H
