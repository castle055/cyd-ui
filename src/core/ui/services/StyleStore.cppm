/*! \file  ui_style.cppm
 *! \brief
 *!
 */

module;
#include "../../../debug/profiling/macros.h"
#define PROF_CURRENT_MODULE cydui::core::ui::services::StyleStore

export module cydui.core.ui.services.StyleStore;

import std;
export import reflect;
export import fabric.services;
import cydui.debug.profiling;

export import cydui.styling;

import cydui.geometric_relations;

namespace cydui::detail::ui::services {
  export class StyleStore final: public fabric::services::ServiceBase {
    tss::StyleArchive::sptr style_archive{tss::StyleArchive::make()};

    StyleStore() = default;

  public:
    static fabric::task<sptr> start(fabric::services::ServiceLocator& locator) {
      co_return sptr{new StyleStore()};
    }

    tss::StyleArchive& get_style_archive() {
      PROF_SCOPE(get_style_archive);
      return *style_archive;
    }

    fabric::task<> attach_stylesheet(const tss::StyleSheet::sptr& style_sheet) {
      PROF_SCOPE(attach_stylesheet);
      style_archive->add_stylesheet(style_sheet);
      co_return;
    }

    fabric::task<> clear_style() {
      PROF_SCOPE(clear_style);
      style_archive->clear();
      co_return;
    }

    void compile_rules_recurse(ComponentImpl& component) {
      PROF_SCOPE(compile_rules);
      style::compile_style_rule_list(component, *style_archive);
      for (auto& child: component.get_children_impl()) {
        compile_rules_recurse(*child);
      }
    }

    void compile_rules(ComponentImpl& component) {
      PROF_SCOPE(compile_rules);
      compile_rules_recurse(component);
    }

    bool update_style(ComponentImpl& root_component) {
      PROF_SCOPE(Update Style);
      return update_component_style_recurse(root_component);
    }

  private:
    bool update_component_style_recurse(ComponentImpl& component) {
      bool style_changed{false};
      {
        PROF_SCOPE(Update Component Style);
        style_changed = style::apply_style(component);

        if (style_changed) {
          component.get_layer().mark_dirty();
          layout::update_component_geometry(component);
        }
      }

      for (auto& child: component.get_children_impl()) {
        style_changed |= update_component_style_recurse(*child);
      }
      return style_changed;
    }
  };
} // namespace cydui
