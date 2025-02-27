/*! \file  stylesheet.cppm
 *! \brief 
 *!
 */

export module cydui.styling.rules;

import std;
import reflect;

import fabric.logging;
export import cydui.styling.selectors;

namespace cydui {
  export struct StyleRule {
    using sptr = std::shared_ptr<StyleRule>;

    std::vector<StyleRuleCombinedSelector> selectors_{};
    std::vector<std::string> relevant_components_{};
    refl::archive properties_{};
  };
}
