//
// Created by castle on 9/3/22.
//

#ifndef CYD_UI_COMPONENT_GEOMETRY_HPP
#define CYD_UI_COMPONENT_GEOMETRY_HPP

#include "../../../events/properties/properties.hpp"

class ComponentGeometry {
public:
  IntProperty x,
              y,
              x_off,
              y_off,
              min_w,
              min_h,
              w,
              h;
  
  IntProperty padding_top,
              padding_right,
              padding_bottom,
              padding_left;
  
  IntProperty margin_top,
              margin_right,
              margin_bottom,
              margin_left;
  
  ComponentGeometry* relative_to = nullptr;
  bool custom_offset = false;
  bool custom_width   = false;
  
  IntProperty::IntBinding abs_x();
  
  IntProperty::IntBinding abs_y();
  
  IntProperty::IntBinding border_x();
  
  IntProperty::IntBinding border_y();
  
  IntProperty::IntBinding content_x();
  
  IntProperty::IntBinding content_y();
  
  IntProperty::IntBinding abs_w();
  
  IntProperty::IntBinding abs_h();
  
  IntProperty::IntBinding border_w();
  
  IntProperty::IntBinding border_h();
  
  IntProperty::IntBinding content_w();
  
  IntProperty::IntBinding content_h();
  
  
  void set_pos(ComponentGeometry* relative, int x, int y);
  
  void set_pos(ComponentGeometry* relative, IntProperty* x, IntProperty* y);
  
  void set_pos(ComponentGeometry* relative, IntProperty::IntBinding x, IntProperty::IntBinding y);
  
  
  void set_size(int w, int h);
  
  void set_size(IntProperty* w, IntProperty* h);
  
  void set_size(IntProperty::IntBinding w, IntProperty::IntBinding h);
};


#endif //CYD_UI_COMPONENT_GEOMETRY_HPP
