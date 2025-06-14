//
// Created by castle on 3/10/25.
//

export module cydui.components.identifier;

import std;
import reflect;

export namespace cydui::components {
  class identifier_t {
  public:
    consteval identifier_t(std::source_location loc = std::source_location::current())
        : loc_(loc) {}

    identifier_t(
      const std::string& id, std::source_location loc = std::source_location::current()
    )
        : loc_(loc),
          computed_id_(id) {}

    identifier_t(
      const char* id, std::source_location loc = std::source_location::current()
    )
        : loc_(loc),
          computed_id_(id) {}

    std::string str() const {
      if (computed_id_.empty()) {
        return compute_id();
      }
      return computed_id_;
    }

    std::string str() {
      if (computed_id_.empty()) {
        computed_id_ = compute_id();
      }
      return computed_id_;
    }

    void set_id(const std::string& id) {
      computed_id_ = id;
    }

    const std::source_location& loc() const {
      return loc_;
    }

  private:
    std::string compute_id() const {
      return std::format("{}:{}:{}", loc_.file_name(), loc_.line(), loc_.column());
    }

  private:
    const std::source_location loc_{};
    std::string computed_id_;
  };
}