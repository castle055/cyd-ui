/*! \file  stylesheet.cppm
 *! \brief 
 *!
 */

export module cydui.styling.selectors;

import std;
import reflect;

import fabric.logging;

namespace cydui {
  export struct StyleRuleSelector {
    std::size_t specificity() const {
      return compute_specificity();
    }

    std::size_t specificity() {
      if (not specificity_.has_value()) {
        specificity_ = compute_specificity();
      }
      return specificity_.value();
    }

    std::string component{};
    std::unordered_set<std::string> tags{};
    std::unordered_set<std::string> pseudo_states{};
  private:
    std::size_t compute_specificity() const {
      return tags.size() + pseudo_states.size();
    }
    [[refl::ignore]]
    std::optional<std::size_t> specificity_ {std::nullopt};
  };


  export struct StyleRuleCombinedSelector {
    enum kind_e {
      FIRST_SELECTOR,
      CHILD_COMBINATOR,
      DESCENDENT_COMBINATOR,
    };

    std::size_t specificity() const {
      return compute_specificity();
    }

    std::size_t specificity() {
      if (not specificity_.has_value()) {
        specificity_ = compute_specificity();
      }
      return specificity_.value();
    }

    std::list<std::pair<kind_e, StyleRuleSelector>> selectors{};
  private:
    std::size_t compute_specificity() const {
      std::size_t s = 0;
      for (const auto & selector : selectors) {
        s += selector.second.specificity();
      }
      return selectors.size() + s;
    }
    [[refl::ignore]]
    std::optional<std::size_t> specificity_ {std::nullopt};
  };
}
