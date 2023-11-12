//
// Created by castle on 10/16/23.
//

#ifndef CYD_UI_VG_H
#define CYD_UI_VG_H

#include "graphics/pixelmap_editor.h"

#include <vector>
#include <variant>

#include "vg_attributes.h"

namespace vg {
    struct vg_element_t {
      virtual ~vg_element_t() = default;
      
      virtual void apply_to(pixelmap_editor_t &editor) const = 0;
    };
    
    struct vg_fragment_t {
      std::vector<vg_element_t*> elements {};
      
      ~vg_fragment_t() {
        for (auto &item: elements) {
          delete item;
        }
      }
      
      bool empty() const {
        return elements.empty();
      }
      
      void append(std::initializer_list<vg_element_t*> _elements) {
        for (const auto &item: _elements) {
          elements.push_back(item);
        }
      }
      
      template<typename ...T>
      void append(T... _elements) {
        static_assert((std::derived_from<T, vg_element_t> && ...), "Elements must derive from vg_element_t.");
        (elements.push_back(new T {_elements}), ...);
      }
    };
    
    template<typename ...T>
    struct accepts_content {
      accepts_content &with(std::vector<std::variant<T...>> _content_) {
        this->content = _content_;
        return *this;
      }
      
      std::vector<std::variant<T...>> content {};
    };
    
    struct line:
      vg_element_t,
      attr_x1<line>,
      attr_y1<line>,
      attr_x2<line>,
      attr_y2<line>,
      attr_opacity<line>,
      attr_stroke<line>,
      attr_stroke_dasharray<line>,
      attr_stroke_dashoffset<line>,
      attr_stroke_linecap<line>,
      attr_stroke_opacity<line>,
      attr_stroke_width<line>,
      attr_cx<line> {
      void apply_to(pixelmap_editor_t &editor) const override {
        editor->set_source_rgba(
          _stroke.color.r,
          _stroke.color.g,
          _stroke.color.b,
          _stroke.color.a
        );
        
        editor->set_line_width(_stroke_width);
        //editor->set_line_cap(_stroke_linecap);
        //editor->set_dash(_stroke_dasharray, _stroke_dashoffset);
        
        editor->move_to(_x1, _y1);
        editor->line_to(_x2, _y2);
        
        editor->stroke();
      }
    };
    
    struct rect:
      vg_element_t,
      attr_cx<line> {
      void apply_to(pixelmap_editor_t &editor) const override {
      
      }
    };
    
    struct circle:
      vg_element_t,
      attr_cx<line> {
      void apply_to(pixelmap_editor_t &editor) const override {
      }
    };
    
    struct group:
      vg_element_t,
      attr_cx<line>,
      accepts_content<line> {
      void apply_to(pixelmap_editor_t &editor) const override {
      }
    };
}


#endif //CYD_UI_VG_H
