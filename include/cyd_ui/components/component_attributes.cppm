// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

export module cydui.components:attributes;

import std;

export import reflect;

export import cydui.dimensions;
import cydui.graphics;

export import :holder;

export {
  struct attribute_i {
    virtual ~attribute_i() = default;
  };

#define COMPONENT_DIM_ATTRIBUTE(TYPE, NAME, DEFAULT)                                               \
  template <typename E>                                                                            \
  struct attr_##NAME: public attribute_i {                                                         \
    TYPE _##NAME;                                                                                  \
    ~attr_##NAME() override = default;                                                             \
    attr_##NAME(const std::shared_ptr<cyd::ui::components::dimension_ctx_t>& ctx)                  \
        : _##NAME(ctx, DEFAULT) {}                                                                 \
    template <typename S = E, typename = std::enable_if_t<!std::is_void_v<S>>>                     \
    inline S& NAME(auto& _##NAME##_) {                                                             \
      this->_##NAME = _##NAME##_;                                                                  \
      return *(E*)this;                                                                            \
    }                                                                                              \
    template <typename S = E, typename = std::enable_if_t<!std::is_void_v<S>>>                     \
    inline S& NAME(auto&& _##NAME##_) {                                                            \
      this->_##NAME = _##NAME##_;                                                                  \
      return *(E*)this;                                                                            \
    }                                                                                              \
    template <typename S = E, typename = std::enable_if_t<std::is_void_v<S>>>                      \
    inline void NAME(auto& _##NAME##_) {                                                           \
      this->_##NAME = _##NAME##_;                                                                  \
    }                                                                                              \
    template <typename S = E, typename = std::enable_if_t<std::is_void_v<S>>>                      \
    inline void NAME(auto&& _##NAME##_) {                                                          \
      this->_##NAME = _##NAME##_;                                                                  \
    }                                                                                              \
                                                                                                   \
    bool operator==(const attr_##NAME& rhl) const {                                                \
      return (this->_##NAME) == (rhl._##NAME);                                                     \
    }                                                                                              \
  }

#define COMPONENT_DIM_ATTRIBUTE_W_MONITOR(TYPE, NAME, DEFAULT)                                     \
  template <typename E>                                                                            \
  struct attr_##NAME: public attribute_i {                                                         \
    TYPE _##NAME;                                                                                  \
    bool _##NAME##_has_changed = false;                                                            \
    ~attr_##NAME() override    = default;                                                          \
    attr_##NAME(const std::shared_ptr<cyd::ui::components::dimension_ctx_t>& ctx)                  \
        : _##NAME(ctx, DEFAULT) {}                                                                 \
    template <typename S = E, typename = std::enable_if_t<!std::is_void_v<S>>>                     \
    inline S& NAME(auto& _##NAME##_) {                                                             \
      this->_##NAME               = _##NAME##_;                                                    \
      this->_##NAME##_has_changed = true;                                                          \
      return *(E*)this;                                                                            \
    }                                                                                              \
    template <typename S = E, typename = std::enable_if_t<!std::is_void_v<S>>>                     \
    inline S& NAME(auto&& _##NAME##_) {                                                            \
      this->_##NAME               = _##NAME##_;                                                    \
      this->_##NAME##_has_changed = true;                                                          \
      return *(E*)this;                                                                            \
    }                                                                                              \
    template <typename S = E, typename = std::enable_if_t<std::is_void_v<S>>>                      \
    inline void NAME(auto& _##NAME##_) {                                                           \
      this->_##NAME               = _##NAME##_;                                                    \
      this->_##NAME##_has_changed = true;                                                          \
    }                                                                                              \
    template <typename S = E, typename = std::enable_if_t<std::is_void_v<S>>>                      \
    inline void NAME(auto&& _##NAME##_) {                                                          \
      this->_##NAME               = _##NAME##_;                                                    \
      this->_##NAME##_has_changed = true;                                                          \
    }                                                                                              \
                                                                                                   \
    bool operator==(const attr_##NAME& rhl) const {                                                \
      return (this->_##NAME) == (rhl._##NAME)                                                      \
             && (this->_##NAME##_has_changed) == (rhl._##NAME##_has_changed);                      \
    }                                                                                              \
  }

  COMPONENT_DIM_ATTRIBUTE(cyd::ui::components::dimension_t, x, {});
  COMPONENT_DIM_ATTRIBUTE(cyd::ui::components::dimension_t, y, {});
  COMPONENT_DIM_ATTRIBUTE_W_MONITOR(cyd::ui::components::dimension_t, width, {});
  COMPONENT_DIM_ATTRIBUTE_W_MONITOR(cyd::ui::components::dimension_t, height, {});

  COMPONENT_DIM_ATTRIBUTE(cyd::ui::components::dimension_t, margin_top, {});
  COMPONENT_DIM_ATTRIBUTE(cyd::ui::components::dimension_t, margin_bottom, {});
  COMPONENT_DIM_ATTRIBUTE(cyd::ui::components::dimension_t, margin_left, {});
  COMPONENT_DIM_ATTRIBUTE(cyd::ui::components::dimension_t, margin_right, {});

  COMPONENT_DIM_ATTRIBUTE(cyd::ui::components::dimension_t, padding_top, {});
  COMPONENT_DIM_ATTRIBUTE(cyd::ui::components::dimension_t, padding_bottom, {});
  COMPONENT_DIM_ATTRIBUTE(cyd::ui::components::dimension_t, padding_left, {});
  COMPONENT_DIM_ATTRIBUTE(cyd::ui::components::dimension_t, padding_right, {});

  using content = std::vector<cyd::ui::components::component_builder_t>;

  template <typename E>
  struct attr_content: public attribute_i {
    template <typename S = E, typename = std::enable_if_t<!std::is_void_v<S>>>
    inline S& operator()(content&& _content_) {
      this->_content = _content_;
      return *(E*)this;
    }
    template <typename S = E, typename = std::enable_if_t<!std::is_void_v<S>>>
    inline S& operator()(content& _content_) {
      this->_content = _content_;
      return *(E*)this;
    }
    template <typename S = E, typename = std::enable_if_t<std::is_void_v<S>>>
    inline void operator()(content&& _content_) {
      this->_content = _content_;
    }
    template <typename S = E, typename = std::enable_if_t<std::is_void_v<S>>>
    inline void operator()(content& _content_) {
      this->_content = _content_;
    }
    content _content{};

    bool operator==(const attr_content& rhl) const {
      if (this->_content.size() != rhl._content.size()) {
        return false;
      }

      // TODO - Should I deep compare every child?

      return true;
    }
  };

  template <typename E>
  struct attr_background: public attribute_i {
    template <typename S = E, typename = std::enable_if_t<!std::is_void_v<S>>>
    inline S& background(vg::paint::solid& _background_) {
      this->_background.paint_data = std::make_unique<vg::paint::solid>(_background_);
      this->_background.paint_type = vg::paint::type::SOLID;
      return *(E*)this;
    }
    template <typename S = E, typename = std::enable_if_t<!std::is_void_v<S>>>
    inline S& background(vg::paint::solid&& _background_) {
      this->_background.paint_data = std::make_unique<vg::paint::solid>(_background_);
      this->_background.paint_type = vg::paint::type::SOLID;
      return *(E*)this;
    }
    template <typename S = E, typename = std::enable_if_t<!std::is_void_v<S>>>
    inline S& background(vg::paint::gradient::linear& _background_) {
      this->_background.paint_data = std::make_unique<vg::paint::gradient::linear>(_background_);
      this->_background.paint_type = vg::paint::type::GRADIENT_LINEAR;
      return *(E*)this;
    }
    template <typename S = E, typename = std::enable_if_t<!std::is_void_v<S>>>
    inline S& background(vg::paint::gradient::linear&& _background_) {
      this->_background.paint_data = std::make_unique<vg::paint::gradient::linear>(_background_);
      this->_background.paint_type = vg::paint::type::GRADIENT_LINEAR;
      return *(E*)this;
    }
    template <typename S = E, typename = std::enable_if_t<!std::is_void_v<S>>>
    inline S& background(vg::paint::gradient::radial& _background_) {
      this->_background.paint_data = std::make_unique<vg::paint::gradient::radial>(_background_);
      this->_background.paint_type = vg::paint::type::GRADIENT_RADIAL;
      return *(E*)this;
    }
    template <typename S = E, typename = std::enable_if_t<!std::is_void_v<S>>>
    inline S& background(vg::paint::gradient::radial&& _background_) {
      this->_background.paint_data = std::make_unique<vg::paint::gradient::radial>(_background_);
      this->_background.paint_type = vg::paint::type::GRADIENT_RADIAL;
      return *(E*)this;
    }


    template <typename S = E, typename = std::enable_if_t<std::is_void_v<S>>>
    inline void background(vg::paint::solid& _background_) {
      this->_background.paint_data = std::make_unique<vg::paint::solid>(_background_);
      this->_background.paint_type = vg::paint::type::SOLID;
    }
    template <typename S = E, typename = std::enable_if_t<std::is_void_v<S>>>
    inline void background(vg::paint::solid&& _background_) {
      this->_background.paint_data = std::make_unique<vg::paint::solid>(_background_);
      this->_background.paint_type = vg::paint::type::SOLID;
    }
    template <typename S = E, typename = std::enable_if_t<std::is_void_v<S>>>
    inline void background(vg::paint::gradient::linear& _background_) {
      this->_background.paint_data = std::make_unique<vg::paint::gradient::linear>(_background_);
      this->_background.paint_type = vg::paint::type::GRADIENT_LINEAR;
    }
    template <typename S = E, typename = std::enable_if_t<std::is_void_v<S>>>
    inline void background(vg::paint::gradient::linear&& _background_) {
      this->_background.paint_data = std::make_unique<vg::paint::gradient::linear>(_background_);
      this->_background.paint_type = vg::paint::type::GRADIENT_LINEAR;
    }
    template <typename S = E, typename = std::enable_if_t<std::is_void_v<S>>>
    inline void background(vg::paint::gradient::radial& _background_) {
      this->_background.paint_data = std::make_unique<vg::paint::gradient::radial>(_background_);
      this->_background.paint_type = vg::paint::type::GRADIENT_RADIAL;
    }
    template <typename S = E, typename = std::enable_if_t<std::is_void_v<S>>>
    inline void background(vg::paint::gradient::radial&& _background_) {
      this->_background.paint_data = std::make_unique<vg::paint::gradient::radial>(_background_);
      this->_background.paint_type = vg::paint::type::GRADIENT_RADIAL;
    }

    bool operator==(const attr_background& rhl) const {
      return _background == rhl._background;
    }

    vg::paint::type _background{vg::paint::type::SOLID, std::make_unique<vg::paint::solid>()};
  };

  template <typename E>
  struct attr_border: public attribute_i {
    template <typename S = E, typename = std::enable_if_t<!std::is_void_v<S>>>
    inline S& border(vg::paint::solid& _border_) {
      this->_border_top.paint_data = std::make_unique<vg::paint::solid>(_border_);
      this->_border_top.paint_type = vg::paint::type::SOLID;

      this->_border_bottom.paint_data = std::make_unique<vg::paint::solid>(_border_);
      this->_border_bottom.paint_type = vg::paint::type::SOLID;

      this->_border_left.paint_data = std::make_unique<vg::paint::solid>(_border_);
      this->_border_left.paint_type = vg::paint::type::SOLID;

      this->_border_right.paint_data = std::make_unique<vg::paint::solid>(_border_);
      this->_border_right.paint_type = vg::paint::type::SOLID;

      return *(E*)this;
    }
    template <typename S = E, typename = std::enable_if_t<!std::is_void_v<S>>>
    inline S& border(vg::paint::solid&& _border_) {
      this->_border_top.paint_data = std::make_unique<vg::paint::solid>(_border_);
      this->_border_top.paint_type = vg::paint::type::SOLID;

      this->_border_bottom.paint_data = std::make_unique<vg::paint::solid>(_border_);
      this->_border_bottom.paint_type = vg::paint::type::SOLID;

      this->_border_left.paint_data = std::make_unique<vg::paint::solid>(_border_);
      this->_border_left.paint_type = vg::paint::type::SOLID;

      this->_border_right.paint_data = std::make_unique<vg::paint::solid>(_border_);
      this->_border_right.paint_type = vg::paint::type::SOLID;

      return *(E*)this;
    }
    template <typename S = E, typename = std::enable_if_t<!std::is_void_v<S>>>
    inline S& border(vg::paint::gradient::linear& _border_) {
      this->_border_top.paint_data = std::make_unique<vg::paint::gradient::linear>(_border_);
      this->_border_top.paint_type = vg::paint::type::GRADIENT_LINEAR;

      this->_border_bottom.paint_data = std::make_unique<vg::paint::gradient::linear>(_border_);
      this->_border_bottom.paint_type = vg::paint::type::GRADIENT_LINEAR;

      this->_border_left.paint_data = std::make_unique<vg::paint::gradient::linear>(_border_);
      this->_border_left.paint_type = vg::paint::type::GRADIENT_LINEAR;

      this->_border_right.paint_data = std::make_unique<vg::paint::gradient::linear>(_border_);
      this->_border_right.paint_type = vg::paint::type::GRADIENT_LINEAR;

      return *(E*)this;
    }
    template <typename S = E, typename = std::enable_if_t<!std::is_void_v<S>>>
    inline S& border(vg::paint::gradient::linear&& _border_) {
      this->_border_top.paint_data = std::make_unique<vg::paint::gradient::linear>(_border_);
      this->_border_top.paint_type = vg::paint::type::GRADIENT_LINEAR;

      this->_border_bottom.paint_data = std::make_unique<vg::paint::gradient::linear>(_border_);
      this->_border_bottom.paint_type = vg::paint::type::GRADIENT_LINEAR;

      this->_border_left.paint_data = std::make_unique<vg::paint::gradient::linear>(_border_);
      this->_border_left.paint_type = vg::paint::type::GRADIENT_LINEAR;

      this->_border_right.paint_data = std::make_unique<vg::paint::gradient::linear>(_border_);
      this->_border_right.paint_type = vg::paint::type::GRADIENT_LINEAR;

      return *(E*)this;
    }
    template <typename S = E, typename = std::enable_if_t<!std::is_void_v<S>>>
    inline S& border(vg::paint::gradient::radial& _border_) {
      this->_border_top.paint_data = std::make_unique<vg::paint::gradient::radial>(_border_);
      this->_border_top.paint_type = vg::paint::type::GRADIENT_RADIAL;

      this->_border_bottom.paint_data = std::make_unique<vg::paint::gradient::radial>(_border_);
      this->_border_bottom.paint_type = vg::paint::type::GRADIENT_RADIAL;

      this->_border_left.paint_data = std::make_unique<vg::paint::gradient::radial>(_border_);
      this->_border_left.paint_type = vg::paint::type::GRADIENT_RADIAL;

      this->_border_right.paint_data = std::make_unique<vg::paint::gradient::radial>(_border_);
      this->_border_right.paint_type = vg::paint::type::GRADIENT_RADIAL;

      return *(E*)this;
    }
    template <typename S = E, typename = std::enable_if_t<!std::is_void_v<S>>>
    inline S& border(vg::paint::gradient::radial&& _border_) {
      this->_border_top.paint_data = std::make_unique<vg::paint::gradient::radial>(_border_);
      this->_border_top.paint_type = vg::paint::type::GRADIENT_RADIAL;

      this->_border_bottom.paint_data = std::make_unique<vg::paint::gradient::radial>(_border_);
      this->_border_bottom.paint_type = vg::paint::type::GRADIENT_RADIAL;

      this->_border_left.paint_data = std::make_unique<vg::paint::gradient::radial>(_border_);
      this->_border_left.paint_type = vg::paint::type::GRADIENT_RADIAL;

      this->_border_right.paint_data = std::make_unique<vg::paint::gradient::radial>(_border_);
      this->_border_right.paint_type = vg::paint::type::GRADIENT_RADIAL;

      return *(E*)this;
    }


    template <typename S = E, typename = std::enable_if_t<std::is_void_v<S>>>
    inline void border(vg::paint::solid& _border_) {
      this->_border_top.paint_data = std::make_unique<vg::paint::solid>(_border_);
      this->_border_top.paint_type = vg::paint::type::SOLID;

      this->_border_bottom.paint_data = std::make_unique<vg::paint::solid>(_border_);
      this->_border_bottom.paint_type = vg::paint::type::SOLID;

      this->_border_left.paint_data = std::make_unique<vg::paint::solid>(_border_);
      this->_border_left.paint_type = vg::paint::type::SOLID;

      this->_border_right.paint_data = std::make_unique<vg::paint::solid>(_border_);
      this->_border_right.paint_type = vg::paint::type::SOLID;
    }
    template <typename S = E, typename = std::enable_if_t<std::is_void_v<S>>>
    inline void border(vg::paint::solid&& _border_) {
      this->_border_top.paint_data = std::make_unique<vg::paint::solid>(_border_);
      this->_border_top.paint_type = vg::paint::type::SOLID;

      this->_border_bottom.paint_data = std::make_unique<vg::paint::solid>(_border_);
      this->_border_bottom.paint_type = vg::paint::type::SOLID;

      this->_border_left.paint_data = std::make_unique<vg::paint::solid>(_border_);
      this->_border_left.paint_type = vg::paint::type::SOLID;

      this->_border_right.paint_data = std::make_unique<vg::paint::solid>(_border_);
      this->_border_right.paint_type = vg::paint::type::SOLID;
    }
    template <typename S = E, typename = std::enable_if_t<std::is_void_v<S>>>
    inline void border(vg::paint::gradient::linear& _border_) {
      this->_border_top.paint_data = std::make_unique<vg::paint::gradient::linear>(_border_);
      this->_border_top.paint_type = vg::paint::type::GRADIENT_LINEAR;

      this->_border_bottom.paint_data = std::make_unique<vg::paint::gradient::linear>(_border_);
      this->_border_bottom.paint_type = vg::paint::type::GRADIENT_LINEAR;

      this->_border_left.paint_data = std::make_unique<vg::paint::gradient::linear>(_border_);
      this->_border_left.paint_type = vg::paint::type::GRADIENT_LINEAR;

      this->_border_right.paint_data = std::make_unique<vg::paint::gradient::linear>(_border_);
      this->_border_right.paint_type = vg::paint::type::GRADIENT_LINEAR;
    }
    template <typename S = E, typename = std::enable_if_t<std::is_void_v<S>>>
    inline void border(vg::paint::gradient::linear&& _border_) {
      this->_border_top.paint_data = std::make_unique<vg::paint::gradient::linear>(_border_);
      this->_border_top.paint_type = vg::paint::type::GRADIENT_LINEAR;

      this->_border_bottom.paint_data = std::make_unique<vg::paint::gradient::linear>(_border_);
      this->_border_bottom.paint_type = vg::paint::type::GRADIENT_LINEAR;

      this->_border_left.paint_data = std::make_unique<vg::paint::gradient::linear>(_border_);
      this->_border_left.paint_type = vg::paint::type::GRADIENT_LINEAR;

      this->_border_right.paint_data = std::make_unique<vg::paint::gradient::linear>(_border_);
      this->_border_right.paint_type = vg::paint::type::GRADIENT_LINEAR;
    }
    template <typename S = E, typename = std::enable_if_t<std::is_void_v<S>>>
    inline void border(vg::paint::gradient::radial& _border_) {
      this->_border_top.paint_data = std::make_unique<vg::paint::gradient::radial>(_border_);
      this->_border_top.paint_type = vg::paint::type::GRADIENT_RADIAL;

      this->_border_bottom.paint_data = std::make_unique<vg::paint::gradient::radial>(_border_);
      this->_border_bottom.paint_type = vg::paint::type::GRADIENT_RADIAL;

      this->_border_left.paint_data = std::make_unique<vg::paint::gradient::radial>(_border_);
      this->_border_left.paint_type = vg::paint::type::GRADIENT_RADIAL;

      this->_border_right.paint_data = std::make_unique<vg::paint::gradient::radial>(_border_);
      this->_border_right.paint_type = vg::paint::type::GRADIENT_RADIAL;
    }
    template <typename S = E, typename = std::enable_if_t<std::is_void_v<S>>>
    inline void border(vg::paint::gradient::radial&& _border_) {
      this->_border_top.paint_data = std::make_unique<vg::paint::gradient::radial>(_border_);
      this->_border_top.paint_type = vg::paint::type::GRADIENT_RADIAL;

      this->_border_bottom.paint_data = std::make_unique<vg::paint::gradient::radial>(_border_);
      this->_border_bottom.paint_type = vg::paint::type::GRADIENT_RADIAL;

      this->_border_left.paint_data = std::make_unique<vg::paint::gradient::radial>(_border_);
      this->_border_left.paint_type = vg::paint::type::GRADIENT_RADIAL;

      this->_border_right.paint_data = std::make_unique<vg::paint::gradient::radial>(_border_);
      this->_border_right.paint_type = vg::paint::type::GRADIENT_RADIAL;
    }


#define BORDER_SIDE(SIDE)                                                                          \
  template <typename S = E, typename = std::enable_if_t<!std::is_void_v<S>>>                       \
  inline S& border_##SIDE(vg::paint::solid& _border_) {                                            \
    this->_border_##SIDE.paint_data = std::make_unique<vg::paint::solid>(_border_);                \
    this->_border_##SIDE.paint_type = vg::paint::type::SOLID;                                      \
    return *(E*)this;                                                                              \
  }                                                                                                \
  template <typename S = E, typename = std::enable_if_t<!std::is_void_v<S>>>                       \
  inline S& border_##SIDE(vg::paint::solid&& _border_) {                                           \
    this->_border_##SIDE.paint_data = std::make_unique<vg::paint::solid>(_border_);                \
    this->_border_##SIDE.paint_type = vg::paint::type::SOLID;                                      \
    return *(E*)this;                                                                              \
  }                                                                                                \
  template <typename S = E, typename = std::enable_if_t<!std::is_void_v<S>>>                       \
  inline S& border_##SIDE(vg::paint::gradient::linear& _border_) {                                 \
    this->_border_##SIDE.paint_data = std::make_unique<vg::paint::gradient::linear>(_border_);     \
    this->_border_##SIDE.paint_type = vg::paint::type::GRADIENT_LINEAR;                            \
    return *(E*)this;                                                                              \
  }                                                                                                \
  template <typename S = E, typename = std::enable_if_t<!std::is_void_v<S>>>                       \
  inline S& border_##SIDE(vg::paint::gradient::linear&& _border_) {                                \
    this->_border_##SIDE.paint_data = std::make_unique<vg::paint::gradient::linear>(_border_);     \
    this->_border_##SIDE.paint_type = vg::paint::type::GRADIENT_LINEAR;                            \
    return *(E*)this;                                                                              \
  }                                                                                                \
  template <typename S = E, typename = std::enable_if_t<!std::is_void_v<S>>>                       \
  inline S& border_##SIDE(vg::paint::gradient::radial& _border_) {                                 \
    this->_border_##SIDE.paint_data = std::make_unique<vg::paint::gradient::radial>(_border_);     \
    this->_border_##SIDE.paint_type = vg::paint::type::GRADIENT_RADIAL;                            \
    return *(E*)this;                                                                              \
  }                                                                                                \
  template <typename S = E, typename = std::enable_if_t<!std::is_void_v<S>>>                       \
  inline S& border_##SIDE(vg::paint::gradient::radial&& _border_) {                                \
    this->_border_##SIDE.paint_data = std::make_unique<vg::paint::gradient::radial>(_border_);     \
    this->_border_##SIDE.paint_type = vg::paint::type::GRADIENT_RADIAL;                            \
    return *(E*)this;                                                                              \
  }                                                                                                \
                                                                                                   \
  template <typename S = E, typename = std::enable_if_t<std::is_void_v<S>>>                       \
  inline void border_##SIDE(vg::paint::solid& _border_) {                                            \
    this->_border_##SIDE.paint_data = std::make_unique<vg::paint::solid>(_border_);                \
    this->_border_##SIDE.paint_type = vg::paint::type::SOLID;                                      \
  }                                                                                                \
  template <typename S = E, typename = std::enable_if_t<std::is_void_v<S>>>                       \
  inline void border_##SIDE(vg::paint::solid&& _border_) {                                           \
    this->_border_##SIDE.paint_data = std::make_unique<vg::paint::solid>(_border_);                \
    this->_border_##SIDE.paint_type = vg::paint::type::SOLID;                                      \
  }                                                                                                \
  template <typename S = E, typename = std::enable_if_t<std::is_void_v<S>>>                       \
  inline void border_##SIDE(vg::paint::gradient::linear& _border_) {                                 \
    this->_border_##SIDE.paint_data = std::make_unique<vg::paint::gradient::linear>(_border_);     \
    this->_border_##SIDE.paint_type = vg::paint::type::GRADIENT_LINEAR;                            \
  }                                                                                                \
  template <typename S = E, typename = std::enable_if_t<std::is_void_v<S>>>                       \
  inline void border_##SIDE(vg::paint::gradient::linear&& _border_) {                                \
    this->_border_##SIDE.paint_data = std::make_unique<vg::paint::gradient::linear>(_border_);     \
    this->_border_##SIDE.paint_type = vg::paint::type::GRADIENT_LINEAR;                            \
  }                                                                                                \
  template <typename S = E, typename = std::enable_if_t<std::is_void_v<S>>>                       \
  inline void border_##SIDE(vg::paint::gradient::radial& _border_) {                                 \
    this->_border_##SIDE.paint_data = std::make_unique<vg::paint::gradient::radial>(_border_);     \
    this->_border_##SIDE.paint_type = vg::paint::type::GRADIENT_RADIAL;                            \
  }                                                                                                \
  template <typename S = E, typename = std::enable_if_t<std::is_void_v<S>>>                       \
  inline void border_##SIDE(vg::paint::gradient::radial&& _border_) {                                \
    this->_border_##SIDE.paint_data = std::make_unique<vg::paint::gradient::radial>(_border_);     \
    this->_border_##SIDE.paint_type = vg::paint::type::GRADIENT_RADIAL;                            \
  }

    BORDER_SIDE(top)
    BORDER_SIDE(bottom)
    BORDER_SIDE(left)
    BORDER_SIDE(right)

    bool operator==(const attr_border& rhl) const {
      return _border_top == rhl._border_top && _border_bottom == rhl._border_bottom
             && _border_left == rhl._border_left && _border_right == rhl._border_right;
    }

    vg::paint::type _border_top{vg::paint::type::SOLID, std::make_unique<vg::paint::solid>()};
    vg::paint::type _border_bottom{vg::paint::type::SOLID, std::make_unique<vg::paint::solid>()};
    vg::paint::type _border_left{vg::paint::type::SOLID, std::make_unique<vg::paint::solid>()};
    vg::paint::type _border_right{vg::paint::type::SOLID, std::make_unique<vg::paint::solid>()};
  };

  template <typename E>
  struct attr_border_width: public attribute_i {
    template <typename TYPE, typename S = E, typename = std::enable_if_t<!std::is_void_v<S>>>
    inline S& border_width(TYPE& _border_width_) {
      this->_border_width_top    = _border_width_;
      this->_border_width_bottom = _border_width_;
      this->_border_width_left   = _border_width_;
      this->_border_width_right  = _border_width_;
      return *(E*)this;
    }
    template <typename TYPE, typename S = E, typename = std::enable_if_t<!std::is_void_v<S>>>
    inline S& border_width(TYPE&& _border_width_) {
      this->_border_width_top    = _border_width_;
      this->_border_width_bottom = _border_width_;
      this->_border_width_left   = _border_width_;
      this->_border_width_right  = _border_width_;
      return *(E*)this;
    }


    template <typename TYPE, typename S = E, typename = std::enable_if_t<std::is_void_v<S>>>
    inline void border_width(TYPE& _border_width_) {
      this->_border_width_top    = _border_width_;
      this->_border_width_bottom = _border_width_;
      this->_border_width_left   = _border_width_;
      this->_border_width_right  = _border_width_;
    }
    template <typename TYPE, typename S = E, typename = std::enable_if_t<std::is_void_v<S>>>
    inline void border_width(TYPE&& _border_width_) {
      this->_border_width_top    = _border_width_;
      this->_border_width_bottom = _border_width_;
      this->_border_width_left   = _border_width_;
      this->_border_width_right  = _border_width_;
    }

#define BORDER_WIDTH_SIDE(SIDE)                                                                    \
  template <typename TYPE, typename S = E, typename = std::enable_if_t<!std::is_void_v<S>>>        \
  inline S& border_width_##SIDE(TYPE& _border_width_) {                                            \
    this->_border_width_##SIDE = _border_width_;                                                   \
    return *(E*)this;                                                                              \
  }                                                                                                \
  template <typename TYPE, typename S = E, typename = std::enable_if_t<!std::is_void_v<S>>>        \
  inline S& border_width_##SIDE(TYPE&& _border_width_) {                                           \
    this->_border_width_##SIDE = _border_width_;                                                   \
    return *(E*)this;                                                                              \
  }                                                                                                \
                                                                                                   \
  template <typename TYPE, typename S = E, typename = std::enable_if_t<std::is_void_v<S>>>        \
  inline void border_width_##SIDE(TYPE& _border_width_) {                                            \
    this->_border_width_##SIDE = _border_width_;                                                   \
  }                                                                                                \
  template <typename TYPE, typename S = E, typename = std::enable_if_t<std::is_void_v<S>>>        \
  inline void border_width_##SIDE(TYPE&& _border_width_) {                                           \
    this->_border_width_##SIDE = _border_width_;                                                   \
  }

    BORDER_WIDTH_SIDE(top)
    BORDER_WIDTH_SIDE(bottom)
    BORDER_WIDTH_SIDE(left)
    BORDER_WIDTH_SIDE(right)

    bool operator==(const attr_border_width& rhl) const {
      return _border_width_top == rhl._border_width_top
             && _border_width_bottom == rhl._border_width_bottom
             && _border_width_left == rhl._border_width_left
             && _border_width_right == rhl._border_width_right;
    }

    int _border_width_top{0};
    int _border_width_bottom{0};
    int _border_width_left{0};
    int _border_width_right{0};
  };

  template <typename E>
  struct attr_border_radius: public attribute_i {
    template <typename TYPE, typename S = E, typename = std::enable_if_t<!std::is_void_v<S>>>
    inline S& border_radius(TYPE& _border_radius_) {
      this->_border_radius_top_left     = _border_radius_;
      this->_border_radius_top_right    = _border_radius_;
      this->_border_radius_bottom_left  = _border_radius_;
      this->_border_radius_bottom_right = _border_radius_;
      return *(E*)this;
    }
    template <typename TYPE, typename S = E, typename = std::enable_if_t<!std::is_void_v<S>>>
    inline S& border_radius(TYPE&& _border_radius_) {
      this->_border_radius_top_left     = _border_radius_;
      this->_border_radius_top_right    = _border_radius_;
      this->_border_radius_bottom_left  = _border_radius_;
      this->_border_radius_bottom_right = _border_radius_;
      return *(E*)this;
    }

    template <typename TYPE, typename S = E, typename = std::enable_if_t<std::is_void_v<S>>>
    inline void border_radius(TYPE& _border_radius_) {
      this->_border_radius_top_left     = _border_radius_;
      this->_border_radius_top_right    = _border_radius_;
      this->_border_radius_bottom_left  = _border_radius_;
      this->_border_radius_bottom_right = _border_radius_;
    }
    template <typename TYPE, typename S = E, typename = std::enable_if_t<std::is_void_v<S>>>
    inline void border_radius(TYPE&& _border_radius_) {
      this->_border_radius_top_left     = _border_radius_;
      this->_border_radius_top_right    = _border_radius_;
      this->_border_radius_bottom_left  = _border_radius_;
      this->_border_radius_bottom_right = _border_radius_;
    }

#define BORDER_RADIUS_SIDE(SIDE)                                                                   \
  template <typename TYPE, typename S = E, typename = std::enable_if_t<!std::is_void_v<S>>>        \
  inline S& border_radius_##SIDE(TYPE& _border_radius_) {                                          \
    this->_border_radius_##SIDE = _border_radius_;                                                 \
    return *(E*)this;                                                                              \
  }                                                                                                \
  template <typename TYPE, typename S = E, typename = std::enable_if_t<!std::is_void_v<S>>>        \
  inline S& border_radius_##SIDE(TYPE&& _border_radius_) {                                         \
    this->_border_radius_##SIDE = _border_radius_;                                                 \
    return *(E*)this;                                                                              \
  }                                                                                                \
                                                                                                   \
  template <typename TYPE, typename S = E, typename = std::enable_if_t<std::is_void_v<S>>>        \
  inline void border_radius_##SIDE(TYPE& _border_radius_) {                                          \
    this->_border_radius_##SIDE = _border_radius_;                                                 \
  }                                                                                                \
  template <typename TYPE, typename S = E, typename = std::enable_if_t<std::is_void_v<S>>>        \
  inline void border_radius_##SIDE(TYPE&& _border_radius_) {                                         \
    this->_border_radius_##SIDE = _border_radius_;                                                 \
  }

    BORDER_RADIUS_SIDE(top_left)
    BORDER_RADIUS_SIDE(top_right)
    BORDER_RADIUS_SIDE(bottom_left)
    BORDER_RADIUS_SIDE(bottom_right)

    bool operator==(const attr_border_radius& rhl) const {
      return _border_radius_top_left == rhl._border_radius_top_left
             && _border_radius_top_right == rhl._border_radius_top_right
             && _border_radius_bottom_left == rhl._border_radius_bottom_left
             && _border_radius_bottom_right == rhl._border_radius_bottom_right;
    }

    int _border_radius_top_left{0};
    int _border_radius_top_right{0};
    int _border_radius_bottom_left{0};
    int _border_radius_bottom_right{0};
  };

  template <typename E>
  struct attr_border_dasharray: public attribute_i {
    template <typename TYPE, typename S = E, typename = std::enable_if_t<!std::is_void_v<S>>>
    inline S& border_dasharray(TYPE& _border_dasharray_) {
      this->_border_dasharray_top     = _border_dasharray_;
      this->_border_dasharray_bottom    = _border_dasharray_;
      this->_border_dasharray_left  = _border_dasharray_;
      this->_border_dasharray_right = _border_dasharray_;
      return *(E*)this;
    }
    template <typename TYPE, typename S = E, typename = std::enable_if_t<!std::is_void_v<S>>>
    inline S& border_dasharray(TYPE&& _border_dasharray_) {
      this->_border_dasharray_top     = _border_dasharray_;
      this->_border_dasharray_bottom    = _border_dasharray_;
      this->_border_dasharray_left  = _border_dasharray_;
      this->_border_dasharray_right = _border_dasharray_;
      return *(E*)this;
    }

    template <typename TYPE, typename S = E, typename = std::enable_if_t<std::is_void_v<S>>>
    inline void border_dasharray(TYPE& _border_dasharray_) {
      this->_border_dasharray_top     = _border_dasharray_;
      this->_border_dasharray_bottom    = _border_dasharray_;
      this->_border_dasharray_left  = _border_dasharray_;
      this->_border_dasharray_right = _border_dasharray_;
    }
    template <typename TYPE, typename S = E, typename = std::enable_if_t<std::is_void_v<S>>>
    inline void border_dasharray(TYPE&& _border_dasharray_) {
      this->_border_dasharray_top     = _border_dasharray_;
      this->_border_dasharray_bottom    = _border_dasharray_;
      this->_border_dasharray_left  = _border_dasharray_;
      this->_border_dasharray_right = _border_dasharray_;
    }


#define BORDER_DASHARRAY_SIDE(SIDE)                                                                \
  template <typename TYPE, typename S = E, typename = std::enable_if_t<!std::is_void_v<S>>>        \
  inline S& border_dasharray_##SIDE(TYPE& _border_dasharray_) {                                    \
    this->_border_dasharray_##SIDE = _border_dasharray_;                                           \
    return *(E*)this;                                                                              \
  }                                                                                                \
  template <typename TYPE, typename S = E, typename = std::enable_if_t<!std::is_void_v<S>>>        \
  inline S& border_dasharray_##SIDE(TYPE&& _border_dasharray_) {                                   \
    this->_border_dasharray_##SIDE = _border_dasharray_;                                           \
    return *(E*)this;                                                                              \
  }                                                                                                \
                                                                                                   \
  template <typename TYPE, typename S = E, typename = std::enable_if_t<std::is_void_v<S>>>         \
  inline void border_dasharray_##SIDE(TYPE& _border_dasharray_) {                                  \
    this->_border_dasharray_##SIDE = _border_dasharray_;                                           \
  }                                                                                                \
  template <typename TYPE, typename S = E, typename = std::enable_if_t<std::is_void_v<S>>>         \
  inline void border_dasharray_##SIDE(TYPE&& _border_dasharray_) {                                 \
    this->_border_dasharray_##SIDE = _border_dasharray_;                                           \
  }

    BORDER_DASHARRAY_SIDE(top)
    BORDER_DASHARRAY_SIDE(bottom)
    BORDER_DASHARRAY_SIDE(left)
    BORDER_DASHARRAY_SIDE(right)

    bool operator==(const attr_border_dasharray& rhl) const {
      return bool(_border_dasharray_top == rhl._border_dasharray_top)
             && bool(_border_dasharray_bottom == rhl._border_dasharray_bottom)
             && bool(_border_dasharray_left == rhl._border_dasharray_left)
             && bool(_border_dasharray_right == rhl._border_dasharray_right);
    }

    std::valarray<double> _border_dasharray_top{};
    std::valarray<double> _border_dasharray_bottom{};
    std::valarray<double> _border_dasharray_left{};
    std::valarray<double> _border_dasharray_right{};
  };


  template <typename E>
  struct attr_opacity: public attribute_i {
    template <typename TYPE, typename S = E, typename = std::enable_if_t<!std::is_void_v<S>>>
    inline S& opacity(TYPE& _opacity_) {
      this->_opacity = _opacity_;
      return *(E*)this;
    }
    template <typename TYPE, typename S = E, typename = std::enable_if_t<!std::is_void_v<S>>>
    inline S& opacity(TYPE&& _opacity_) {
      this->_opacity = _opacity_;
      return *(E*)this;
    }

    template <typename TYPE, typename S = E, typename = std::enable_if_t<std::is_void_v<S>>>
    inline void opacity(TYPE& _opacity_) {
      this->_opacity = _opacity_;
    }
    template <typename TYPE, typename S = E, typename = std::enable_if_t<std::is_void_v<S>>>
    inline void opacity(TYPE&& _opacity_) {
      this->_opacity = _opacity_;
    }

    bool operator==(const attr_opacity& rhl) const {
      return _opacity == rhl._opacity;
    }

    double _opacity{1.0};
  };

  template <typename E>
  struct attr_rotation: public attribute_i {
    template <typename TYPE, typename S = E, typename = std::enable_if_t<!std::is_void_v<S>>>
    inline S& rotation(TYPE& _rotation_) {
      this->_rotation = _rotation_;
      return *(E*)this;
    }
    template <typename TYPE, typename S = E, typename = std::enable_if_t<!std::is_void_v<S>>>
    inline S& rotation(TYPE&& _rotation_) {
      this->_rotation = _rotation_;
      return *(E*)this;
    }

    template <typename TYPE, typename S = E, typename = std::enable_if_t<std::is_void_v<S>>>
    inline void rotation(TYPE& _rotation_) {
      this->_rotation = _rotation_;
    }
    template <typename TYPE, typename S = E, typename = std::enable_if_t<std::is_void_v<S>>>
    inline void rotation(TYPE&& _rotation_) {
      this->_rotation = _rotation_;
    }

    bool operator==(const attr_rotation& rhl) const {
      return _rotation == rhl._rotation;
    }

    double _rotation{0.0};
  };


  template <typename T>
  struct attrs_margin: attr_margin_top<T>,
                       attr_margin_bottom<T>,
                       attr_margin_left<T>,
                       attr_margin_right<T> {
    explicit attrs_margin(const std::shared_ptr<cyd::ui::components::dimension_ctx_t>& dimension_ctx
    )
        : attr_margin_top<T>(dimension_ctx),
          attr_margin_bottom<T>(dimension_ctx),
          attr_margin_left<T>(dimension_ctx),
          attr_margin_right<T>(dimension_ctx) {}

    bool operator==(const attrs_margin& rhl) const {
      return attr_margin_top<T>::operator==(rhl) && attr_margin_bottom<T>::operator==(rhl)
             && attr_margin_left<T>::operator==(rhl) && attr_margin_right<T>::operator==(rhl);
    }
  };

  template <typename T>
  struct attrs_padding: attr_padding_top<T>,
                        attr_padding_bottom<T>,
                        attr_padding_left<T>,
                        attr_padding_right<T> {
    explicit attrs_padding(
      const std::shared_ptr<cyd::ui::components::dimension_ctx_t>& dimension_ctx
    )
        : attr_padding_top<T>(dimension_ctx),
          attr_padding_bottom<T>(dimension_ctx),
          attr_padding_left<T>(dimension_ctx),
          attr_padding_right<T>(dimension_ctx) {}

    bool operator==(const attrs_padding& rhl) const {
      return attr_padding_top<T>::operator==(rhl) && attr_padding_bottom<T>::operator==(rhl)
             && attr_padding_left<T>::operator==(rhl) && attr_padding_right<T>::operator==(rhl);
    }
  };

  template <typename T>
  struct attrs_border: attr_border<T>, attr_border_width<T>, attr_border_radius<T>, attr_border_dasharray<T> {
    bool operator==(const attrs_border& rhl) const {
      return attr_border<T>::operator==(rhl) && attr_border_width<T>::operator==(rhl)
             && attr_border_radius<T>::operator==(rhl) && attr_border_dasharray<T>::operator==(rhl);
    }
  };

  template <typename T = void>
  struct attrs_dimensions: attr_x<T>,
                           attr_y<T>,
                           attr_width<T>,
                           attr_height<T>,
                           attrs_margin<T>,
                           attrs_padding<T> {
    bool operator==(const attrs_dimensions& rhl) const {
      return attr_x<T>::operator==(rhl) && attr_y<T>::operator==(rhl)
             && attr_width<T>::operator==(rhl) && attr_height<T>::operator==(rhl)
             && attrs_margin<T>::operator==(rhl) && attrs_padding<T>::operator==(rhl);
    }

    explicit attrs_dimensions(
      std::shared_ptr<cyd::ui::components::dimension_ctx_t> dimension_ctx_ =
        std::make_shared<cyd::ui::components::dimension_ctx_t>()
    )
        : attr_x<T>(dimension_ctx_),
          attr_y<T>(dimension_ctx_),
          attr_width<T>(dimension_ctx_),
          attr_height<T>(dimension_ctx_),
          attrs_margin<T>(dimension_ctx_),
          attrs_padding<T>(dimension_ctx_),
          dimension_ctx(dimension_ctx_) {}

    attrs_dimensions(const attrs_dimensions& rhl)
        : attr_x<T>(rhl.dimension_ctx),
          attr_y<T>(rhl.dimension_ctx),
          attr_width<T>(rhl.dimension_ctx),
          attr_height<T>(rhl.dimension_ctx),
          attrs_margin<T>(rhl.dimension_ctx),
          attrs_padding<T>(rhl.dimension_ctx),
          dimension_ctx(rhl.dimension_ctx) {
      attr_x<T>::operator=(rhl);
      attr_y<T>::operator=(rhl);
      attr_width<T>::operator=(rhl);
      attr_height<T>::operator=(rhl);
      attrs_margin<T>::operator=(rhl);
      attrs_padding<T>::operator=(rhl);
    }

    attrs_dimensions& operator=(const attrs_dimensions& rhl) {
      attr_x<T>::operator=(rhl);
      attr_y<T>::operator=(rhl);
      attr_width<T>::operator=(rhl);
      attr_height<T>::operator=(rhl);
      attrs_margin<T>::operator=(rhl);
      attrs_padding<T>::operator=(rhl);

      return *this;
    }

    attrs_dimensions& update_with(const attrs_dimensions& rhl) {
      attr_x<T>::operator=(rhl);
      attr_y<T>::operator=(rhl);
      attr_width<T>::operator=(rhl);
      attr_height<T>::operator=(rhl);
      attrs_margin<T>::operator=(rhl);
      attrs_padding<T>::operator=(rhl);

      return *this;
    }

  protected:
    std::shared_ptr<cyd::ui::components::dimension_ctx_t> dimension_ctx;
  };

  template <typename T = void>
  struct attrs_component: attrs_dimensions<T>,
                          attrs_border<T>,
                          attr_content<T>,
                          attr_opacity<T>,
                          attr_rotation<T>,
                          attr_background<T> {
    bool operator==(const attrs_component& rhl) const {
      return attrs_dimensions<T>::operator==(rhl) && attr_border<T>::operator==(rhl)
             && attr_content<T>::operator==(rhl) && attr_opacity<T>::operator==(rhl)
             && attr_rotation<T>::operator==(rhl) && attr_background<T>::operator==(rhl);
    }

    attrs_component& update_with(const attrs_component& rhl) {
      attr_content<T>::operator=(rhl);
      attr_opacity<T>::operator=(rhl);
      attr_rotation<T>::operator=(rhl);
      attr_background<T>::operator=(rhl);
      attrs_border<T>::operator=(rhl);
      attrs_dimensions<T>::update_with(rhl);

      return *this;
    }
  };

#undef COMPONENT_ATTRIBUTE
#undef COMPONENT_ATTRIBUTE_W_MONITOR
}
