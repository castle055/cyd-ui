/*! \file  stylesheet.cppm
 *! \brief 
 *!
 */

export module cydui.styling;

import std;
import reflect;

import fabric.logging;
import cydui.styling.tss.parser;

export import cydui.styling.selectors;
export import cydui.styling.rules;


namespace cyd::ui {
  template<typename O>
  void print_ast_node(O &o, const fabric::node_t *node, int indent = 0) {
    for (int i = 0; i < indent; ++i) {
      o << ' ';
    }
    if (node->is_type<syntax::tss::tss_decl_color_literal>()) {
      auto* c = node->as<syntax::tss::tss_decl_color_literal>();
      o << "<color hex=" << std::format("{:?}", c->data.to_string()) << "/>" << std::endl;
      return;
    }
    if (node->is_text) {
      o << "<text text=" << std::format("{:?}", node->text) << "/>" << std::endl;
      return;
    }

    o << "<" << node->text;
    if (node->children.empty()) {
      o << "/>" << std::endl;
    } else {
      if (node->children.size() == 1 && node->children[0]->is_text) {
        o << " text=" << std::format("{:?}", node->children[0]->text) << "/>" << std::endl;
        return;
      }

      o << ">" << std::endl;
      for (const auto &child: node->children) {
        print_ast_node(o, child.get(), indent + 1);
      }

      for (int i = 0; i < indent; ++i) {
        o << ' ';
      }

      o << "</" << node->text << ">" << std::endl;
    }
  }

  export class ast_visitor {
    void visit(const syntax::tss::tss_decl_color_literal::sptr& c) {
      // c->color;
    }
  };

  export class StyleSheet {
  public:
    using sptr = std::shared_ptr<StyleSheet>;

    static sptr make() {
      return std::make_shared<StyleSheet>();
    }

    static sptr parse(auto input) {
      auto [ok, ast, log] = lang::tss::parse(input);

      if (not ok) {
        LOG::print {ERROR}("Couldn't parse stylesheet");
        return std::make_shared<StyleSheet>();
      }
      // print_ast_node(std::cout, ast.get(), 0);

      sptr ptr = std::make_shared<StyleSheet>();
      for (const auto& rule : ast->data.rules) {
        for (const auto& key : rule->relevant_components_) {
          ptr->rule_map_[key].push_back(rule);
        }
      }

      return ptr;
    }

    auto& rules_by_component(const std::string& component_name) const {
      if (rule_map_.contains(component_name)) {
        return rule_map_.at(component_name);
      }
      return empty_list_;
    }

    auto& any_rules() const {
      return any_rules_;
    }
  private:
    std::unordered_map<std::string, std::list<StyleRule::sptr>> rule_map_{};
    std::list<StyleRule::sptr> any_rules_{};
    std::list<StyleRule::sptr> empty_list_{};
  };

  export class StyleArchive {
  public:
    using sptr = std::shared_ptr<StyleArchive>;

    static sptr make() {
      return std::make_shared<StyleArchive>();
    }

    void add_stylesheet(const StyleSheet::sptr& sheet) {
      style_sheets_.emplace_back(sheet);
    }

    void for_each_rule(const std::string& component_name, auto&& func) const {
      for (const auto & style_sheet : style_sheets_) {
        for (const auto& rule: style_sheet->any_rules()) {
          func(rule);
        }
        for (const auto& rule: style_sheet->rules_by_component(component_name)) {
          func(rule);
        }
      }
    }

  private:
    std::vector<StyleSheet::sptr> style_sheets_{};
  };
}
