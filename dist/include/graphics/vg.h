//
// Created by castle on 10/16/23.
//

#ifndef CYD_UI_VG_H
#define CYD_UI_VG_H

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
    
    // ? Basic Shapes
    // * <line>
    struct line:
      vg_element_t,
      attrs_core<line>,
      attrs_stroke<line>,
      attr_x1<line>,
      attr_y1<line>,
      attr_x2<line>,
      attr_y2<line> {
      line() {
        // per element Defaults
        this->stroke_width(1);
      }
      
      void apply_to(pixelmap_editor_t &editor) const override {
        apply_stroke(editor);
        
        editor->set_source_rgba(
          _stroke.color.r,
          _stroke.color.g,
          _stroke.color.b,
          _stroke.color.a * _stroke_opacity
        );
        
        editor->move_to(_x1, _y1);
        editor->line_to(_x2, _y2);
        
        editor->stroke();
      }
    };
    
    // * <polygon>
    struct polygon:
      vg_element_t,
      attrs_core<polygon>,
      attrs_fill<polygon>,
      attrs_stroke<polygon>,
      attr_points<polygon> {
      void apply_to(pixelmap_editor_t &editor) const override {
        apply_stroke(editor);
        apply_fill(editor);
        
        bool first = true;
        for (const auto &p: _points) {
          if (first) {
            editor->move_to(p[0], p[1]);
            first = false;
          } else {
            editor->line_to(p[0], p[1]);
          }
        }
        // Close the polygon
        editor->close_path();
        
        editor->set_source_rgba(
          _stroke.color.r,
          _stroke.color.g,
          _stroke.color.b,
          _stroke.color.a * _stroke_opacity
        );
        editor->stroke_preserve();
        
        editor->set_source_rgba(
          _fill.color.r,
          _fill.color.g,
          _fill.color.b,
          _fill.color.a * _fill_opacity
        );
        editor->fill();
      }
    };
    
    // * <polyline>
    struct polyline:
      vg_element_t,
      attrs_core<polyline>,
      attrs_fill<polyline>,
      attrs_stroke<polyline>,
      attr_points<polyline> {
      void apply_to(pixelmap_editor_t &editor) const override {
        apply_stroke(editor);
        apply_fill(editor);
        
        bool first = true;
        for (const auto &p: _points) {
          if (first) {
            editor->move_to(p[0], p[1]);
            first = false;
          } else {
            editor->line_to(p[0], p[1]);
          }
        }
        
        editor->set_source_rgba(
          _stroke.color.r,
          _stroke.color.g,
          _stroke.color.b,
          _stroke.color.a * _stroke_opacity
        );
        editor->stroke_preserve();
        
        editor->set_source_rgba(
          _fill.color.r,
          _fill.color.g,
          _fill.color.b,
          _fill.color.a * _fill_opacity
        );
        editor->fill();
      }
    };
    
    // * <path>
    struct path:
      vg_element_t,
      attrs_core<path>,
      attrs_fill<path>,
      attrs_stroke<path>,
      attr_path_str<path> {
      void apply_to(pixelmap_editor_t &editor) const override {
        apply_stroke(editor);
        apply_fill(editor);
        
      }
    };
    
    // * <rect>
    struct rect:
      vg_element_t,
      attrs_core<rect>,
      attrs_fill<rect>,
      attrs_stroke<rect>,
      attr_x<rect>,
      attr_y<rect>,
      attr_width<rect>,
      attr_height<rect>,
      attr_rx<rect>,
      attr_ry<rect> {
      void apply_to(pixelmap_editor_t &editor) const override {
        apply_stroke(editor);
        apply_fill(editor);
        
        // Replace with a proper path construction that takes into account
        // _rx and _ry
        if (_rx == 0 && _ry == 0) {
          editor->rectangle(_x, _y, _width, _height);
          editor->set_source_rgba(
            _stroke.color.r,
            _stroke.color.g,
            _stroke.color.b,
            _stroke.color.a * _stroke_opacity
          );
          editor->stroke_preserve();
          
          editor->set_source_rgba(
            _fill.color.r,
            _fill.color.g,
            _fill.color.b,
            _fill.color.a * _fill_opacity
          );
          editor->fill();
        } else {
          editor->rectangle(
            _x + _rx,
            _y,
            _width - _rx * 2,
            _height
          );
          editor->rectangle(
            _x,
            _y + _ry,
            _rx,
            _height - _ry * 2
          );
          editor->rectangle(
            _width - _rx,
            _y + _ry,
            _rx,
            _height - _ry * 2
          );
        }
      }
    };
    
    // * <circle>
    struct circle:
      vg_element_t,
      attrs_core<circle>,
      attrs_fill<circle>,
      attrs_stroke<circle>,
      attr_cx<circle>,
      attr_cy<circle>,
      attr_r<circle> {
      void apply_to(pixelmap_editor_t &editor) const override {
        apply_stroke(editor);
        apply_fill(editor);
        
        editor->arc(_cx, _cy, _r, 0, 2 * M_PI);
        editor->set_source_rgba(
          _stroke.color.r,
          _stroke.color.g,
          _stroke.color.b,
          _stroke.color.a * _stroke_opacity
        );
        editor->stroke_preserve();
        
        editor->set_source_rgba(
          _fill.color.r,
          _fill.color.g,
          _fill.color.b,
          _fill.color.a * _fill_opacity
        );
        editor->fill();
      }
    };
    
    // * <ellipse>
    struct ellipse:
      vg_element_t,
      attrs_core<ellipse>,
      attrs_fill<ellipse>,
      attrs_stroke<ellipse>,
      attr_cx<ellipse>,
      attr_cy<ellipse>,
      attr_rx<ellipse>,
      attr_ry<ellipse> {
      void apply_to(pixelmap_editor_t &editor) const override {
        apply_stroke(editor);
        apply_fill(editor);
        
        editor->save();
        editor->translate(_cx, _cy);
        editor->scale(_rx, _ry);
        editor->arc(0.0, 0.0, 1.0, 0.0, 2 * M_PI);
        
        editor->set_source_rgba(
          _stroke.color.r,
          _stroke.color.g,
          _stroke.color.b,
          _stroke.color.a * _stroke_opacity
        );
        editor->stroke_preserve();
        
        editor->set_source_rgba(
          _fill.color.r,
          _fill.color.g,
          _fill.color.b,
          _fill.color.a * _fill_opacity
        );
        editor->fill();
        
        editor->restore();
      }
    };
    
    // ? Group
    // * <g> - Group
    struct group:
      vg_element_t,
      attr_cx<group>,
      attrs_core<group>,
      attrs_fill<ellipse>,
      attrs_stroke<ellipse>,
      accepts_content<line> {
      void apply_to(pixelmap_editor_t &editor) const override {
        apply_stroke(editor);
      }
    };
}


// * <image>

// * <linearGradient>
// * <radialGradient>

// * <text>
// * <textPath>
// * <tspan>

// * <mask>
// * <clipPath>

// * <defs>

// * <filter>
// * <feBlend>
// * <feColorMatrix>
// * <feComponentTransfer>
// * <feComposite>
// * <feConvolveMatrix>
// * <feDiffuseLighting>
// * <feDisplacementMap>
// * <feDistantLight>
// * <feDropShadow>
// * <feFlood>
// * <feFuncA>
// * <feFuncB>
// * <feFuncG>
// * <feFuncR>
// * <feGaussianBlur>
// * <feImage>
// * <feMerge>
// * <feMergeNode>
// * <feMorphology>
// * <feOffset>
// * <fePointLight>
// * <feSpecularLighting>
// * <feSpotLight>
// * <feTile>
// * <feTurbulence>

// * <animate>
// * <animateMotion>
// * <animateTransform>
// * <stop>

// * <pattern>

// * <svg>
// * <a>
// * <cursor>
// * <foreignObject>
// * <glyph>
// * <glyphRef>
// * <marker>
// * <metadata>
// * <missing-glyph>
// * <mpath>
// * <script>
// * <set>
// * <style>
// * <switch>
// * <symbol>
// * <title> — the SVG accessible name element
// * <desc>
// * <tref>
// * <use>
// * <view>
// * <hkern>
// * <vkern>

// * <font>
// * <font-face-format>
// * <font-face-name>
// * <font-face-src>
// * <font-face-uri>
// * <font-face>

#endif //CYD_UI_VG_H
