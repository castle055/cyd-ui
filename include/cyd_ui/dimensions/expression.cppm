/*! \file  expression.cppm
 *! \brief 
 *!
 */

export module cydui.dimensions:expression;

import std;

import fabric.logging;

import :types;

template <typename Type>
class cyd::ui::dimensions::expression {
public:
  using dimension = dimension<Type>;
  using parameter = parameter<Type>;
  class node_t {
  public:
    using sptr = std::shared_ptr<node_t>;

    enum operation {
      ADDITION,
      SUBTRACTION,
      MULTIPLICATION,
      DIVISION,
      CONSTANT,
      DIMENSION,
      PARAMETER,
    } op;

    explicit node_t(operation op_)
        : op(op_) {
      switch (op) {
        case DIMENSION:
          dimension = nullptr;
          break;
        case ADDITION:
        case SUBTRACTION:
        case MULTIPLICATION:
        case DIVISION:
          children = {};
          break;
        case CONSTANT:
        case PARAMETER:
        default:
          break;
      }
    }

    ~node_t() {
      switch (op) {
        case DIMENSION:
          dimension.reset();
          break;
        case ADDITION:
        case SUBTRACTION:
        case MULTIPLICATION:
        case DIVISION:
          children.clear();
          break;
        case CONSTANT:
        case PARAMETER:
        default:
          break;
      }
    }

    static sptr make(operation op_) {
      return std::make_shared<node_t>(op_);
    }

    bool operator==(const node_t& other) const {
      if (op != other.op)
        return false;

      switch (op) {
        case CONSTANT:
          return (const_value == other.const_value);
        case DIMENSION:
          return (dimension == other.dimension);
        case PARAMETER:
          return (parameter == other.parameter);
        case ADDITION:
        case SUBTRACTION:
        case MULTIPLICATION:
        case DIVISION:
          if (children.size() != other.children.size())
            return false;
          for (auto it1 = children.begin(), it2 = other.children.begin(); it1 != children.end();
               ++it1, ++it2) {
            if ((*it1 != *it2) or (*(*it1).get() != *(*it2).get()))
              return false;
          }
          return true;
      }
    }

    std::string to_string() const {
      std::stringstream sts{};
      switch (op) {
        case CONSTANT:
          sts << const_value;
          break;
        case DIMENSION:
          sts << std::format("[0x{:X}](", (unsigned long)dimension.get());
          sts << dimension->value_ << ")";
          break;
        case PARAMETER:
          sts << std::format("[{}]", parameter.name);
          break;
        case ADDITION:
        case SUBTRACTION:
        case MULTIPLICATION:
        case DIVISION:
          if (children.empty()) {
            sts << "()";
          } else {
            sts << "(";
            auto it = children.begin();
            sts << it->get()->to_string();
            ++it;
            for (; it != children.end(); ++it) {
              switch (op) {
                case ADDITION:
                  sts << " + ";
                  break;
                case SUBTRACTION:
                  sts << " - ";
                  break;
                case MULTIPLICATION:
                  sts << " * ";
                  break;
                case DIVISION:
                  sts << " / ";
                  break;
                default:
                  break;
              }
              sts << it->get()->to_string();
            }
            sts << ")";
          }
          break;
      }
      return sts.str();
    }

    Type                                  const_value{};
    std::shared_ptr<dimension_impl<Type>> dimension{nullptr};
    std::list<sptr>                       children{};
    parameter                             parameter{};
  };

  expression() = default;
  expression(const Type& value) {
    auto node = make_node(node_t::CONSTANT);
    node->const_value = value;
    tree_ = node;
  }

  void add_dependency(std::shared_ptr<dimension_impl<Type>> dependency) {
    dependencies_.insert(dependency);
  }

  static typename node_t::sptr make_node(node_t::operation op) { return node_t::make(op); }

  bool operator==(const expression& other) const {
    if (other.dependencies_.size() != dependencies_.size())
        return false;

    for (const auto& dep: dependencies_) {
        if (not other.dependencies_.contains(dep))
            return false;
    }
    
    if (tree_ == other.tree_) {
      return true;
    } else if (tree_ == nullptr || other.tree_ == nullptr) {
      return false;
    } else {
      return (*tree_.get() == *other.tree_.get());
    }
  }

  std::string to_string() const {
    if (tree_ == nullptr) {
      return "default";
    } else {
      return tree_->to_string();
    }
  }

  bool empty() const {
    return tree_.get() == nullptr;
  }

  void clear() {
    tree_.reset();
    tree_ = {nullptr};
    dependencies_.clear();
  }

  auto& tree() {
    return tree_;
  }

  const auto& tree() const {
    return tree_;
  }

  auto& dependencies() {
    return dependencies_;
  }

  const auto& dependencies() const {
    return dependencies_;
  }

  friend struct dimensional_operators;
  friend dimension_impl<Type>;

private:
  typename node_t::sptr tree_ = nullptr;
  std::unordered_set<std::shared_ptr<dimension_impl<Type>>> dependencies_{};
};
