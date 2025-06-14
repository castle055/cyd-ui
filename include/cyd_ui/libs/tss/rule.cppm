/*! \file  stylesheet.cppm
 *! \brief
 *!
 */

export module cydui.styling.lang.rules;

import std;
import reflect;

import fabric.logging;
export import cydui.styling.lang.selectors;

namespace cydui {
  export struct StyleRule {
    using sptr = std::shared_ptr<StyleRule>;

    std::vector<StyleRuleCombinedSelector> selectors_{};
    std::vector<std::string>               relevant_components_{};
    refl::archive                          properties_{};
  };

  export struct StyleRuleInstance {
    std::size_t     specificity;
    StyleRule::sptr rule;
    bool            active{false};

    bool operator==(const StyleRuleInstance& other) const {
      return this->specificity == other.specificity and this->rule == other.rule
             and this->active == other.active;
    }
  };

} // namespace cydui
