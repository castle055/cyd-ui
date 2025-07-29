/*! \file  UI_options.cppm
 *! \brief
 *!
 */

export module cydui.ui_options;

import std;
import fabric.logging;

export import cydui.styling.lang;


namespace cydui {
  export struct UIOptions {
    std::vector<tss::StyleSheet::sptr> stylesheets;

    void attach_stylesheet(const std::filesystem::path& style_sheet) {
      stylesheets.push_back(tss::StyleSheet::parse(style_sheet));
    }

    void attach_style(const std::string& style_str) {
      stylesheets.push_back(tss::StyleSheet::parse(style_str));
    }
  };
} // namespace cydui
