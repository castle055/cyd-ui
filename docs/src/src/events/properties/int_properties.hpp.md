//
// Created by castle on 9/3/22.
//

#ifndef CYD_UI_INT_PROPERTIES_HPP
#define CYD_UI_INT_PROPERTIES_HPP

class IntProperty: public Property {
public:
  struct IntBinding {
    IntProperty* property;
    
    int val() {
      return property->val();
    }
    
    // Similar to `val()`
    // For inline values like `if (c->state->geom.border_x().compute())`
    // Prevents memory leaks
    bool is_computed = false;
    
    int compute() {
      int computed_val = property->val();
      if (!property->persistent) {
        delete property;
        property = new IntProperty(computed_val);
        property->persistent = true;
        is_computed = true;
      }
      return computed_val;
    }
    
    IntProperty* unwrap() {
      auto* p = property;
      property = new IntProperty(p->val());
      property->persistent = true;
      is_computed = true;
      return p;
    }
    
    ~IntBinding() {
      if (!property->persistent || is_computed)
        delete property;
    }
    
    // PLUS +
    IntBinding operator+(int i) {
      return (*unwrap()) + i;
    }
    
    IntBinding operator+(IntProperty &i) {
      return (*unwrap()) + i;
    }
    
    IntBinding operator+(IntBinding i) {
      return (*unwrap()) + i;
    }
    
    // MINUS -
    IntBinding operator-(int i) {
      return (*unwrap()) - i;
    }
    
    IntBinding operator-(IntProperty &i) {
      return (*unwrap()) - i;
    }
    
    IntBinding operator-(IntBinding i) {
      return (*unwrap()) - i;
    }
    
    // MULTIPLY *
    IntBinding operator*(int i) {
      return (*unwrap()) * i;
    }
    
    IntBinding operator*(IntProperty &i) {
      return (*unwrap()) * i;
    }
    
    IntBinding operator*(IntBinding i) {
      return (*unwrap()) * i;
    }
    
    // DIVIDE /
    IntBinding operator/(int i) {
      return (*unwrap()) / i;
    }
    
    IntBinding operator/(IntProperty &i) {
      return (*unwrap()) / i;
    }
    
    IntBinding operator/(IntBinding i) {
      return (*unwrap()) / i;
    }
  };
  
  
  explicit IntProperty(): Property() {
    value = new int(0);
  }
  
  explicit IntProperty(int v): Property() {
    value = new int(0);
    this->binding = [this, v]() {
      this->set_val(v);
    };
    this->binding();
  }

#pragma clang diagnostic push
#pragma ide diagnostic ignored "google-explicit-constructor"
  
  IntProperty(IntBinding b): Property() {
    value = new int(0);
    auto* b_prop = b.unwrap();
    this->binding = [this, b_prop]() {
      this->set_val(b_prop->val());
    };
    this->binding();
    b_prop->addListener(this);
  }

#pragma clang diagnostic pop
  
  
  ~IntProperty() override {
    delete ((int*)value);
    value = nullptr;
    //clearDependencies();
  }
  
  void set_val(int i) {
    int* v = (int*)value;
    if (!v) {
      value = new int(i);
    } else {
      *v = i;
    }
  }
  
  IntProperty &operator=(int i) {
    clearDependencies();
    int prev = val();
    set_val(i);
    binding = []() { };
    
    if (prev != i)
      update();
    
    return *this;
  }
  
  IntProperty &operator=(IntBinding b) {
    clearDependencies();
    auto* b_prop = b.unwrap();
    this->binding = [this, b_prop]() {
      this->set_val(b_prop->val());
    };
    int prev = val();
    this->binding();
    int _new = val();
    b_prop->addListener(this);
    
    if (prev != _new)
      update();
    
    return *this;
  }
  
  void set_binding(std::function<int()> updater, std::vector<Property*> deps) {
    clearDependencies();
    this->binding = [this, updater]() {
      this->set_val(updater());
    };
    int prev      = val();
    this->binding();
    int _new = val();
    
    for (auto &d: deps)
      d->addListener(this);
    
    //if (prev != _new)
    //  update();
  }
  
  int val() {
    int* v = (int*)value;
    if (v)
      return *v;
    else
      return 0;
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

protected:
  bool is_equal(void* new_val) override {
    return *(int*)new_val == val();
  }
  
  bool changed_value() override {
    int prev = val();
    binding();
    int _new = val();
    return _new != prev;
  }

private:
  IntBinding do_operation(std::function<int()> op) {
    auto* prop = new IntProperty(0);
    prop->persistent = false;
    
    prop->binding = [prop, op]() {
      prop->set_val(op());
    };
    prop->binding();
    
    addListener(prop);
    return {
        .property = prop
    };
  }

public:
  
  // PLUS +
  IntBinding operator+(int i) {
    return do_operation([this, i]() { return this->val() + i; });
  }
  
  IntBinding operator+(IntProperty &i) {
    auto b = do_operation([this, &i]() { return this->val() + i.val(); });
    i.addListener(b.property);
    return b;
  }
  
  IntBinding operator+(IntBinding i) {
    auto* prop = i.unwrap();
    auto b = do_operation([this, prop]() { return this->val() + prop->val(); });
    prop->addListener(b.property);
    return b;
  }
  
  // MINUS -
  IntBinding operator-(int i) {
    return do_operation([this, i]() { return this->val() - i; });
  }
  
  IntBinding operator-(IntProperty &i) {
    auto b = do_operation([this, &i]() { return this->val() - i.val(); });
    i.addListener(b.property);
    return b;
  }
  
  IntBinding operator-(IntBinding i) {
    auto* prop = i.unwrap();
    auto b = do_operation([this, prop]() { return this->val() - prop->val(); });
    prop->addListener(b.property);
    return b;
  }
  
  // MULTIPLY *
  IntBinding operator*(int i) {
    return do_operation([this, i]() { return this->val() * i; });
  }
  
  IntBinding operator*(IntProperty &i) {
    auto b = do_operation([this, &i]() { return this->val() * i.val(); });
    i.addListener(b.property);
    return b;
  }
  
  IntBinding operator*(IntBinding i) {
    auto* prop = i.unwrap();
    auto b = do_operation([this, prop]() { return this->val() * prop->val(); });
    prop->addListener(b.property);
    return b;
  }
  
  // DIVIDE /
  IntBinding operator/(int i) {
    return do_operation([this, i]() { return i == 0? 0 : this->val() / i; });
  }
  
  IntBinding operator/(IntProperty &i) {
    auto b = do_operation([this, &i]() { return i.val() == 0? 0 : this->val() / i.val(); });
    i.addListener(b.property);
    return b;
  }
  
  IntBinding operator/(IntBinding i) {
    auto* prop = i.unwrap();
    auto b = do_operation([this, prop]() { return prop->val() == 0? 0 : this->val() / prop->val(); });
    prop->addListener(b.property);
    return b;
  }
};

#endif //CYD_UI_INT_PROPERTIES_HPP
