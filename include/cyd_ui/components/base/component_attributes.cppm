// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

export module cydui.components.base:attributes;

import std;

export import reflect;

export import cydui.dimensions;
import cydui.graphics;

export import :holder;

export
{
  struct attribute_i {
    virtual ~attribute_i() = default;
  };

  using content = std::vector<cyd::ui::components::component_builder_t>;

  template<typename E>
  struct attr_content: public attribute_i {
    template<typename S = E, typename = std::enable_if_t<!std::is_void_v<S>>>
    inline S &operator()(content &&_content_) {
      this->_content = _content_;
      return *(E*)this;
    }

    template<typename S = E, typename = std::enable_if_t<!std::is_void_v<S>>>
    inline S &operator()(content &_content_) {
      this->_content = _content_;
      return *(E*)this;
    }

    template<typename S = E, typename = std::enable_if_t<std::is_void_v<S>>>
    inline void operator()(content &&_content_) {
      this->_content = _content_;
    }

    template<typename S = E, typename = std::enable_if_t<std::is_void_v<S>>>
    inline void operator()(content &_content_) {
      this->_content = _content_;
    }

    content _content { };

    bool operator==(const attr_content &rhl) const {
      if (this->_content.size() != rhl._content.size()) {
        return false;
      }

      // TODO - Should I deep compare every child?

      return true;
    }
  };

  template<typename T = void>
  struct attrs_component: attr_content<T> {
    bool operator==(const attrs_component &rhl) const {
      return attr_content<T>::operator==(rhl);
    }

    attrs_component &update_with(const attrs_component &rhl) {
      attr_content<T>::operator=(rhl);

      return *this;
    }
  };

#undef COMPONENT_ATTRIBUTE
#undef COMPONENT_ATTRIBUTE_W_MONITOR
}
