/*! \file  ui_style.cppm
 *! \brief
 *!
 */

module;
#include "../../debug/profiling/macros.h"
#define PROF_CURRENT_MODULE cydui::core::style

export module cydui.core.style;

import std;
export import reflect;
import cydui.debug.profiling;

export import cydui.styling;

import cydui.geometric_relations;

namespace cydui::core {
  export class StyleStore {
    tss::StyleArchive::sptr style_archive{tss::StyleArchive::make()};

  public:
    StyleStore() = default;

    tss::StyleArchive& get_style_archive() {
      PROF_SCOPE(get_style_archive);
      return *style_archive;
    }

    void attach_stylesheet(const tss::StyleSheet::sptr& style_sheet) {
      PROF_SCOPE(attach_stylesheet);
      style_archive->add_stylesheet(style_sheet);
    }

    void compile_rules_recurse(mounted_component_t& component) {
      PROF_SCOPE(compile_rules);
      compile_rules(component);
      for (auto& child: component.get_children()) {
        compile_rules_recurse(*child);
      }
    }

    void compile_rules(mounted_component_t& component) {
      PROF_SCOPE(compile_rules);
      style::compile_style_rule_list(component, *style_archive);
    }

    bool update_style(mounted_component_t& root_component) {
      PROF_SCOPE(Update Style);
      return update_component_style_recurse(root_component);
    }

  private:
    bool update_component_style_recurse(mounted_component_t& component) {
      bool style_changed{false};
      {
        PROF_SCOPE(Update Component Style);
        style_changed = style::apply_style(component);

        if (style_changed) {
          component.get_compositing_node().queue_graphics_update();
          layout::update_component_geometry(component);
        }
      }

      for (auto& child: component.get_children()) {
        style_changed |= update_component_style_recurse(*child);
      }
      return style_changed;
    }
  };
} // namespace cydui
