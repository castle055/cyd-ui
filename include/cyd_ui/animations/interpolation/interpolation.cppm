//
// Created by castle on 2/21/25.
//

export module cydui.interpolation;

import std;
import fabric.logging;

export import cydui.easing_functions;

export import :mappings;

#define FLOAT_TOLERANCE 0.0001

bool fuzzy_eq(const float a, const float b) {
  return std::abs(a - b) < FLOAT_TOLERANCE;
}

bool is_zero(float a) {
  return fuzzy_eq(a, 0.0f);
}

std::optional<float> find_root(std::function<float(float)> fun, float x0, float x1) {
  static constexpr int N_MAX = 1000;
  int n = N_MAX;

  if (is_zero(fun(x0))) {
    // LOG::print{INFO}("OK: {} iterations", N_MAX - n);
    return x0;
  }
  if (is_zero(fun(x1))) {
    // LOG::print{INFO}("OK: {} iterations", N_MAX - n);
    return x1;
  }

  while (n-- > 0) {
    float x = (x0 + x1) / 2;
    float y = fun(x);
    // LOG::print{INFO}("[ITER: {}] error: {}", N_MAX - n, y);
    if (is_zero(y)) {
      // LOG::print{INFO}("OK: {} iterations", N_MAX - n);
      return x;
    }

    if (y < 0.0f) {
      x0 = x;
    } else {
      x1 = x;
    }
  }

  LOG::print{ERROR}("FAILED to find root");
  return std::nullopt;
}

namespace cyd::ui {
  export class interpolator_base {
  public:
    using sptr = std::shared_ptr<interpolator_base>;

    virtual ~interpolator_base() = default;
    virtual float_type interpolate(float_type x0, float_type x1, float_type t) const = 0;
  };

  namespace interp {
    export class lerp final: public interpolator_base {
    public:
      lerp() = default;
      float_type interpolate(float_type x0, float_type x1, float_type t) const override {
        return (1 - t) * x0 + t * x1;
      }
    };

    export class easing final: public interpolator_base {
    public:
      using function_type = cyd::ui::easing::function_type;
      explicit easing(function_type fun): fun_(fun) {}
      float_type interpolate(float_type x0, float_type x1, float_type t) const override {
        const float_type s = normalize(t, fun_);
        return (1 - s) * x0 + s * x1;
      }
    private:
      static float_type normalize(float_type t, const function_type& fun) {
        return (fun(t) - fun(0)) / (fun(1) - fun(0));
      }

    private:
      function_type fun_;
    };

    export class bezier final: public interpolator_base {
    public:
      struct control_point {
        float_type t{};
        float_type x{};
      };
      bezier(control_point c0_, control_point c1_)
          : c0(c0_),
            c1(c1_) {
        // Clamp control points within time domain [0, 1]
        c0.t = std::clamp(c0.t, 0.0f, 1.0f);
        c1.t = std::clamp(c1.t, 0.0f, 1.0f);

        // Cache values used in the cubic bezier polynomial
        As = 3 * c0.t;
        Bs = 3 * (-2 * c0.t + c1.t);
        // Cs = (-3 * c0.t + c1.t + 3);
        Cs = (3 * c0.t - 3 * c1.t + 1);
      }
      float_type interpolate(float_type x0, float_type x1, float_type t) const override {
        const auto s_opt = find_root([&](const float_type s) {
            return s * As           //
                   + s * s * Bs     //
                   + s * s * s * Cs //
                   - t;
        }, 0.0f, 1.0f);

        if (s_opt.has_value()) {
          const float_type s = s_opt.value();
          // I'm translating the curve so that x0 = 0 and the control points
          // are specified relative to their respective end points:
          // c0 relative to x0, and c1 relative to x1
          const float_type xd = x1 - x0;
          return x0 + (s * 3 * (c0.x - x0)         //
                 + s * s * 3 * (x0 - 2 * c0.x + (x1 + c1.x)) //
                 + s * s * s * (-x0 + 3 * c0.x - 3 * (x1 + c1.x) + x1));

          // Generic curve
          // return x0                                 //
          //        + s * 3 * (x1 - x0)                //
          //        + s * s * 3 * (x0 - 2 * x1 + c0.x) //
          //        + s * s * s * (-x0 + 3 * x1 - 3 * c0.x + c1.x);
        }
        return x0;
      }
    private:
      control_point c0{};
      control_point c1{};

      float_type As, Bs, Cs;
    };
  }
} // namespace cyd::ui
