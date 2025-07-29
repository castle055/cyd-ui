// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

export module cydui.transform;

import std;
import reflect;

export import cydui.elements;
export import fabric.linalg;


export namespace cydui {
  template <std::size_t N>
  using vec  = la::vec<N>;
  using vec2 = la::vec<2>;
  using vec3 = la::vec<3>;
  using vec4 = la::vec<4>;

  template <std::size_t R, std::size_t C = R>
  using mat  = la::mat<R, C>;
  using mat2 = la::mat<2, 2>;
  using mat3 = la::mat<3, 3>;
  using mat4 = la::mat<4, 4>;

  struct AffineTransform {
    mat2 linear;
    vec2 translation;

    AffineTransform()
        : linear(mat2::identity()),
          translation(vec2::zero()) {}

    AffineTransform(const AffineTransform&)            = default;
    AffineTransform(AffineTransform&&)                 = default;
    AffineTransform& operator=(const AffineTransform&) = default;
    AffineTransform& operator=(AffineTransform&&)      = default;

    static AffineTransform identity() {
      return AffineTransform {};
    }

    static AffineTransform from_translation(vec2 translation) {
      AffineTransform res {identity()};
      res.translation = translation;
      return res;
    }

    static AffineTransform from_scaling(vec2 scale) {
      AffineTransform res {identity()};
      res.linear[0, 0] = scale[0];
      res.linear[1, 1] = scale[1];
      return res;
    }

    static AffineTransform from_shear(vec2 shear) {
      AffineTransform res {identity()};
      res.linear[1, 0] = shear[0];
      res.linear[0, 1] = shear[1];
      return res;
    }

    static AffineTransform from_rotation(la::scalar angle) {
      AffineTransform res {identity()};
      res.linear = {
        {std::cos(angle), -std::sin(angle)},
        {std::sin(angle),  std::cos(angle)},
      };
      return res;
    }

    AffineTransform operator*(const AffineTransform& other) const {
      AffineTransform result {*this};
      result.linear *= other.linear;
      result.translation[0] +=
        this->linear[0, 0] * other.translation[0] + this->linear[0, 1] * other.translation[1];
      result.translation[1] +=
        this->linear[1, 0] * other.translation[0] + this->linear[1, 1] * other.translation[1];
      return result;
    }

    vec2 operator*(const vec2& v) const {
      return linear * v + translation;
    }

    AffineTransform translate(vec2 translation) const {
      // AffineTransform result {*this};
      // result.translation[0] *= translation[0];
      // result.translation[1] *= translation[1];
      // return result;
      return (*this) * from_translation(translation);
    }

    AffineTransform scale(vec2 scale) const {
      return (*this) * from_scaling(scale);
    }

    AffineTransform rotate(
      quantify::quantity<
        quantify::angle::radians,
        double> angle) const {
      return (*this) * from_rotation(angle.value);
    }

    AffineTransform shear(vec2 shear) const {
      return (*this) * from_shear(shear);
    }

    AffineTransform reflect_x() const {
      AffineTransform result {*this};
      result.linear[0, 0] *= -1;
      return result;
    }

    AffineTransform reflect_y() const {
      AffineTransform result {*this};
      result.linear[1, 1] *= -1;
      return result;
    }
  };
} // namespace cydui
