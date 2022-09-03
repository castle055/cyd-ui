//
// Created by castle on 9/3/22.
//

#ifndef CYD_UI_INT_PROPERTIES_H
#define CYD_UI_INT_PROPERTIES_H

class IntProperty: public Property {
  int val() {
    return *(int*)value;
  }
  
  void set_val(int i) {
    *(int*)value = i;
  }

public:
  explicit IntProperty(): Property() {
    value = new int;
  }
  
  ~IntProperty() override {
    delete ((int*)value);
  }
  
  void set_raw_value(void* val) override {
    value = val;
    update();
  }
  
  void set(int i) {
    if (val() != i) {
      cydui::events::emit(
          new cydui::events::CEvent {
              .type = cydui::events::EVENT_LAYOUT,
              .data = new cydui::events::layout::CLayoutEvent {
                  .type = cydui::events::layout::LYT_EV_UPDATE_PROP,
                  .data = cydui::events::layout::CLayoutData {
                      .update_prop_ev = cydui::events::layout::CUpdatePropEvent {
                          .target_property = this,
                          .new_value = new int(i)
                      }
                  }
              }
          }
      );
    }
  }
  
  // PLUS +
  IntProperty* operator+(int i) {
    auto* prop = new IntProperty;
    
    prop->binding = [prop, this, i]() {
      prop->set_val(this->val() + i);
    };
    
    addListener(prop);
    return prop;
  }
  
  IntProperty* operator+(IntProperty &i) {
    auto* prop = new IntProperty;
    
    prop->binding = [prop, this, &i]() {
      prop->set_val(this->val() + i.val());
    };
    
    addListener(prop);
    i.addListener(prop);
    return prop;
  }
  
  // MINUS -
  IntProperty* operator-(int i) {
    auto* prop = new IntProperty;
    
    prop->binding = [prop, this, i]() {
      prop->set_val(this->val() - i);
    };
    
    addListener(prop);
    return prop;
  }
  
  IntProperty* operator-(IntProperty &i) {
    auto* prop = new IntProperty;
    
    prop->binding = [prop, this, &i]() {
      prop->set_val(this->val() - i.val());
    };
    
    addListener(prop);
    i.addListener(prop);
    return prop;
  }
  
  // MULTIPLY *
  IntProperty* operator*(int i) {
    auto* prop = new IntProperty;
    
    prop->binding = [prop, this, i]() {
      prop->set_val(this->val() * i);
    };
    
    addListener(prop);
    return prop;
  }
  
  IntProperty* operator*(IntProperty &i) {
    auto* prop = new IntProperty;
    
    prop->binding = [prop, this, &i]() {
      prop->set_val(this->val() * i.val());
    };
    
    addListener(prop);
    i.addListener(prop);
    return prop;
  }
  
  // DIVIDE /
  IntProperty* operator/(int i) {
    auto* prop = new IntProperty;
    
    prop->binding = [prop, this, i]() {
      prop->set_val(this->val() / i);
    };
    
    addListener(prop);
    return prop;
  }
  
  IntProperty* operator/(IntProperty &i) {
    auto* prop = new IntProperty;
    
    prop->binding = [prop, this, &i]() {
      prop->set_val(this->val() / i.val());
    };
    
    addListener(prop);
    i.addListener(prop);
    return prop;
  }
};

#endif //CYD_UI_INT_PROPERTIES_H
