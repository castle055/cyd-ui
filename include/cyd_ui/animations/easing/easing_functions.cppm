//
// Created by castle on 2/21/25.
//

export module cydui.easing_functions;

import std;

export namespace cydui {
  using float_type = float;
}

export namespace cydui::easing {
  using function_type = std::function<float_type(const float_type&)>;

  constexpr float_type linear(const float_type& t) { return t; }
}