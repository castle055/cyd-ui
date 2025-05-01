/*! \file  digraph.cppm
 *! \brief
 *!
 */

export module cydui.digraph;

import std;
export import reflect;

namespace cydui {
  export template <typename T>
  class digraph_node;

  export template <typename T>
  concept DigraphNode = std::derived_from<T, digraph_node<T>>;

  namespace digraph {
    export template <DigraphNode T>
    typename T::sptr make_node() {
      auto ptr   = std::make_shared<T>();
      ptr->self_ = std::static_pointer_cast<digraph_node<T>>(ptr);
      return ptr;
    }
  }

  template <typename T>
  class digraph_node {
  public:
    using value_type = T;
    using ptr        = digraph_node*;
    using wptr       = std::weak_ptr<digraph_node>;
    using sptr       = std::shared_ptr<digraph_node>;

  private:
    [[refl::ignore]]
    std::list<sptr> inputs_{};
    [[refl::ignore]]
    std::list<ptr> outputs_{};

    [[refl::ignore]]
    wptr self_{};

  protected:
    digraph_node() = default;

  public:
    template <DigraphNode S>
    friend typename S::sptr digraph::make_node();

    digraph_node(const digraph_node&)            = delete;
    digraph_node& operator=(const digraph_node&) = delete;

    digraph_node(digraph_node&& other) = delete;
    digraph_node& operator=(digraph_node&& other) = delete;

    ~digraph_node() {
      clear_inputs();
    }

  private:
    bool has_input(const sptr& other) {
      for (const auto& input: inputs_) {
        if (input.get() == other.get()) {
          return true;
        }
      }
      return false;
    }
    bool has_output(const sptr& other) {
      for (const auto& output: outputs_) {
        if (output == other.get()) {
          return true;
        }
      }
      return false;
    }
    void connect_input(const sptr& other) {
      if (has_input(other)) {
        return;
      }
      inputs_.emplace_back(other);
    }
    void disconnect_input(const sptr& other) {
      inputs_.remove(other);
    }
    void connect_output(const sptr& other) {
      if (has_output(other)) {
        return;
      }
      outputs_.emplace_back(other.get());
    }
    void disconnect_output(digraph_node* other) {
      outputs_.remove(other);
    }
    void disconnect_output(const sptr& other) {
      outputs_.remove(other.get());
    }
    void add_output(const sptr& other) {
      connect_output(other);
      other->connect_input(self_.lock());
    }
    void clear_outputs() {
      auto self = self_.lock();
      for (auto& output: outputs_) {
        output->disconnect_input(self);
      }
      outputs_.clear();
    }

  public:
    void add_input(const sptr& other) {
      connect_input(other);
      other->connect_output(self_.lock());
    }

    void clear_inputs() {
      for (const auto& input: inputs_) {
        input->disconnect_output(this);
      }
      inputs_.clear();
    }

    const auto& get_inputs() {
      return inputs_;
    }
    const auto& get_outputs() {
      return outputs_;
    }

    auto& data() {
      return *static_cast<T*>(this);
    }
    auto data_sptr() {
      return std::static_pointer_cast<T>(self_.lock());
    }
  };
} // namespace cydui
