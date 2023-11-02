//
// Created by castle on 10/26/23.
//

#ifndef CYD_UI_COMPONENT_ATTRIBUTES_H
#define CYD_UI_COMPONENT_ATTRIBUTES_H

#define COMPONENT_ATTRIBUTE(TYPE, NAME, DEFAULT) \
    template<typename E>                  \
    struct attr_##NAME {                  \
      inline E &NAME(TYPE& _##NAME##_) {  \
        this->_##NAME = _##NAME##_;       \
        return *(E*)this;                 \
      }                                   \
      inline E &NAME(TYPE&& _##NAME##_) { \
        this->_##NAME = _##NAME##_;       \
        return *(E*)this;                 \
      }                                   \
      TYPE _##NAME = DEFAULT;             \
    }

COMPONENT_ATTRIBUTE(int, x, 0);

COMPONENT_ATTRIBUTE(int, y, 0);

COMPONENT_ATTRIBUTE(int, w, 0);

COMPONENT_ATTRIBUTE(int, h, 0);

#undef COMPONENT_ATTRIBUTE

#endif //CYD_UI_COMPONENT_ATTRIBUTES_H
