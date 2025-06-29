/*! \file  ui_style.cppm
 *! \brief
 *!
 */

module;
#include "cyd_ui/debug/profiling/macros.h"
#define PROF_CURRENT_MODULE cydui::layout::ui_style

export module cydui.layout.ui_style;

import std;
export import reflect;
import cydui.debug.profiling;

export import cydui.styling.actions.compile_style_rule_list;
export import cydui.styling.actions.apply_style;

import cydui.geometric_relations;

namespace cydui {
  export class ui_style {
    tss::StyleArchive::sptr style_archive{tss::StyleArchive::make()};

  public:
    ui_style() = default;

    tss::StyleArchive& get_style_archive() {
      PROF_SCOPE(get_style_archive);
      return *style_archive;
    }

    void attach_stylesheet(const tss::StyleSheet::sptr& style_sheet) {
      PROF_SCOPE(attach_stylesheet);
      style_archive->add_stylesheet(style_sheet);
    }

    void compile_rules_recurse(components::mounted_component_t& component) {
      PROF_SCOPE(compile_rules);
      compile_rules(component);
      for (auto & child : component.get_children()) {
        compile_rules_recurse(*child);
      }
    }

    void compile_rules(components::mounted_component_t& component) {
      PROF_SCOPE(compile_rules);
      styling::compile_style_rule_list(component, *style_archive);
    }

    void update_style(components::mounted_component_t& root_component) {
      PROF_SCOPE(Update Style);
      update_component_style_recurse(root_component);
    }

  private:
    void update_component_style_recurse(components::mounted_component_t& component) {
      {
        PROF_SCOPE(Update Component Style);
        bool style_changed = styling::apply_style(component);

        if (style_changed) {
          component.get_compositing_node().queue_graphics_update();
          geometry::update_component_geometry(component);
        }
      }

      for (auto& child: component.get_children()) {
        update_component_style_recurse(*child);
      }
    }
  };
} // namespace cydui
