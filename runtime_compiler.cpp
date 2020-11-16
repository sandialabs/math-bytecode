#include "parsegen_language.hpp"
#include "parsegen_reader.hpp"

#include <iostream>

namespace rtc {

enum token : std::size_t {
  token_integer,
  token_floating_point,
  token_plus,
  token_minus,
  token_times,
  token_divide,
  token_assign,
  token_open_subexpression,
  token_close_subexpression,
  token_open_array,
  token_close_array,
  token_double,
  token_identifier,
  token_statement_end,
  token_argument_separator,
  token_count
};

enum production : std::size_t {
  production_program,
  production_first_statement,
  production_next_statement,
  production_assign,
  production_declare_assign,
  production_declare_scalar,
  production_declare_array,
  production_variable,
  production_array_entry,
  production_type_double,
  production_sum_or_difference,
  production_product_or_quotient,
  production_decay_to_negation,
  production_decay_to_leaf,
  production_read,
  production_subexpression,
  production_unary_call,
  production_binary_call,
  production_sum,
  production_difference,
  production_product,
  production_quotient,
  production_negation,
  production_literal,
  production_count
};

parsegen::language build_language() {
  parsegen::language l;
  std::string const space_regex = "[ \t\r\n]*";
  std::string const int_regex = "(0|([1-9][0-9]*))";
  l.tokens.resize(token_count);
  l.tokens[token_integer] = {
    "integer", int_regex + space_regex };
  l.tokens[token_floating_point] = {
    "floating_point", int_regex + "\\.[0-9]*([eE][\\-\\+]?[0-9]+)?" + space_regex };
  l.tokens[token_plus] = {"plus", "\\+" + space_regex};
  l.tokens[token_minus] = {"minus", "\\-" + space_regex};
  l.tokens[token_times] = {"times", "\\*" + space_regex};
  l.tokens[token_divide] = {"divide", "/" + space_regex};
  l.tokens[token_assign] = {"assign", "=" + space_regex};
  l.tokens[token_open_subexpression] = {"open_subexpression", "\\(" + space_regex};
  l.tokens[token_close_subexpression] = {"close_subexpression", "\\)" + space_regex};
  l.tokens[token_open_array] = {"open_array", "\\[" + space_regex};
  l.tokens[token_close_array] = {"close_array", "\\]" + space_regex};
  l.tokens[token_double] = {"double", "double" + space_regex};
  l.tokens[token_identifier] = {"identifier", "[_A-Za-z][_A-Za-z0-9]*" + space_regex};
  l.tokens[token_statement_end] = {"statement_end", ";" + space_regex};
  l.tokens[token_argument_separator] = {"argument_separator", "," + space_regex};
  l.productions.resize(production_count);
  l.productions[production_program] =
  {"program", {"statements"}};
  l.productions[production_first_statement] =
  {"statements", {"statement"}};
  l.productions[production_next_statement] =
  {"statements", {"statements", "statement"}};
  l.productions[production_assign] =
  {"statement", {"mutable", "assign", "immutable", "statement_end"}};
  l.productions[production_declare_assign] =
  {"statement", {"type", "identifier", "assign", "immutable", "statement_end"}};
  l.productions[production_declare_scalar] =
  {"statement", {"type", "identifier", "statement_end"}};
  l.productions[production_declare_array] =
  {"statement", {"type", "identifier", "open_array", "integer", "close_array", "statement_end"}};
  l.productions[production_variable] =
  {"mutable", {"identifier"}};
  l.productions[production_array_entry] =
  {"mutable", {"identifier", "open_array", "integer", "close_array"}};
  l.productions[production_type_double] =
  {"type", {"double"}};
  l.productions[production_sum_or_difference] =
  {"immutable", {"sum_or_difference"}};
  l.productions[production_product_or_quotient] =
  {"sum_or_difference", {"product_or_quotient"}};
  l.productions[production_decay_to_negation] =
  {"product_or_quotient", {"negation"}};
  l.productions[production_decay_to_leaf] =
  {"negation", {"leaf"}};
  l.productions[production_read] =
  {"leaf", {"mutable"}};
  l.productions[production_subexpression] =
  {"leaf", {"open_subexpression", "immutable", "close_subexpression"}};
  l.productions[production_unary_call] =
  {"leaf", {"identifier", "open_subexpression", "immutable", "close_subexpression"}};
  l.productions[production_binary_call] =
  {"leaf", {"identifier", "open_subexpression", "immutable", "argument_separator", "immutable", "close_subexpression"}};
  l.productions[production_sum] =
  {"sum_or_difference", {"sum_or_difference", "plus", "product_or_quotient"}};
  l.productions[production_difference] =
  {"sum_or_difference", {"sum_or_difference", "minus", "product_or_quotient"}};
  l.productions[production_product] =
  {"product_or_quotient", {"product_or_quotient", "times", "negation"}};
  l.productions[production_quotient] =
  {"product_or_quotient", {"product_or_quotient", "divide", "negation"}};
  l.productions[production_negation] =
  {"negation", {"minus", "leaf"}};
  l.productions[production_literal] =
  {"leaf", {"floating_point"}};
  return l;
}

static inline std::string remove_trailing_space(std::string s) {
  s.erase(std::find_if(s.rbegin(), s.rend(), [](char ch) {
    return !std::isspace(ch);
  }).base(), s.end());
  return s;
}

enum class instruction_code {
  copy,
  add,
  subtract,
  multiply,
  divide,
  negate,
  assign_constant,
  sqrt,
};

class named_instruction {
 public:
  instruction_code code;
  std::string result_name;
  std::string left_name;
  std::string right_name;
  double constant;
};

std::ostream& operator<<(
    std::ostream& s, named_instruction const& op)
{
  switch (op.code) {
    case instruction_code::copy:
    {
      s << op.result_name << " = " << op.left_name << '\n';
      break;
    }
    case instruction_code::add:
    {
      s << op.result_name << " = "
        << op.left_name << " + "
        << op.right_name << '\n';
      break;
    }
    case instruction_code::subtract:
    {
      s << op.result_name << " = "
        << op.left_name << " - "
        << op.right_name << '\n';
      break;
    }
    case instruction_code::multiply:
    {
      s << op.result_name << " = "
        << op.left_name << " * "
        << op.right_name << '\n';
      break;
    }
    case instruction_code::divide:
    {
      s << op.result_name << " = "
        << op.left_name << " / "
        << op.right_name << '\n';
      break;
    }
    case instruction_code::negate:
    {
      s << op.result_name << " = -"
        << op.left_name << '\n';
      break;
    }
    case instruction_code::assign_constant:
    {
      s << op.result_name << " = "
        << op.constant << '\n';
      break;
    }
    case instruction_code::sqrt:
    {
      s << op.result_name << " = sqrt("
        << op.left_name << ")\n";
    }
  }
  return s;
}

class reader : public parsegen::reader
{
 public:
  reader():
    parsegen::reader(
        parsegen::build_reader_tables(
          rtc::build_language()))
  {
  }
  virtual std::any at_shift(int token, std::string& text) override
  {
    switch (token) {
      case token_identifier: return remove_trailing_space(std::move(text));
      case token_integer: return std::stoi(text);
      case token_floating_point: return std::stod(text);
    }
    return std::any();
  }
  virtual std::any at_reduce(
      int production, std::vector<std::any>& rhs) override
  {
    switch (production) {
      case production_program:
      {
        for (std::size_t i = 0; i < instructions.size(); ++i) {
          std::cout << i << ": " << instructions[i];
        }
        compute_live_ranges();
        for (auto& lr : live_ranges) {
          std::cout << lr.name << " at register " << lr.register_assigned
            << " from " << lr.when_written_to << " to " << lr.when_last_read << '\n';
        }
        break;
      }
      case production_assign:
      {
        named_instruction op;
        op.code = instruction_code::copy;
        op.result_name = std::any_cast<std::string&&>(std::move(rhs.at(0)));
        op.left_name = std::any_cast<std::string&&>(std::move(rhs.at(2)));
        instructions.push_back(op);
        break;
      }
      case production_declare_assign:
      {
        named_instruction op;
        op.code = instruction_code::copy;
        op.result_name = std::any_cast<std::string&&>(std::move(rhs.at(1)));
        op.left_name = std::any_cast<std::string&&>(std::move(rhs.at(3)));
        instructions.push_back(op);
        break;
      }
      case production_variable:
      {
        return std::move(rhs.at(0));
      }
      case production_array_entry:
      {
        auto array_name = std::any_cast<std::string&&>(std::move(rhs.at(0)));
        auto index = std::any_cast<int>(rhs.at(2));
        auto entry_name = array_name + "[" + std::to_string(index) + "]";
        return entry_name;
      }
      case production_sum_or_difference:
      case production_product_or_quotient:
      case production_decay_to_negation:
      case production_decay_to_leaf:
      case production_read:
      {
        return std::move(rhs.at(0));
      }
      case production_subexpression:
      {
        return std::move(rhs.at(1));
      }
      case production_unary_call:
      {
        auto result = get_temporary();
        auto function_name =
          remove_trailing_space(
              std::any_cast<std::string&&>(
                std::move(rhs.at(0))));
        named_instruction op;
        op.result_name = result;
        if (function_name == "sqrt") {
          op.code = instruction_code::sqrt;
        } else {
          throw parsegen::parse_error("unknown function name");
        }
        op.left_name =
          std::any_cast<std::string&&>(
              std::move(rhs.at(2)));
        instructions.push_back(op);
        return result;
      }
      case production_sum:
      {
        auto result = get_temporary();
        named_instruction op;
        op.code = instruction_code::add;
        op.result_name = result;
        op.left_name = std::any_cast<std::string&&>(std::move(rhs.at(0)));
        op.right_name = std::any_cast<std::string&&>(std::move(rhs.at(2)));
        instructions.push_back(op);
        return result;
      }
      case production_difference:
      {
        auto result = get_temporary();
        named_instruction op;
        op.code = instruction_code::subtract;
        op.result_name = result;
        op.left_name = std::any_cast<std::string&&>(std::move(rhs.at(0)));
        op.right_name = std::any_cast<std::string&&>(std::move(rhs.at(2)));
        instructions.push_back(op);
        return result;
      }
      case production_product:
      {
        auto result = get_temporary();
        named_instruction op;
        op.code = instruction_code::multiply;
        op.result_name = result;
        op.left_name = std::any_cast<std::string&&>(std::move(rhs.at(0)));
        op.right_name = std::any_cast<std::string&&>(std::move(rhs.at(2)));
        instructions.push_back(op);
        return result;
      }
      case production_quotient:
      {
        auto result = get_temporary();
        named_instruction op;
        op.code = instruction_code::divide;
        op.result_name = result;
        op.left_name = std::any_cast<std::string&&>(std::move(rhs.at(0)));
        op.right_name = std::any_cast<std::string&&>(std::move(rhs.at(2)));
        instructions.push_back(op);
        return result;
      }
      case production_negation:
      {
        auto result = get_temporary();
        named_instruction op;
        op.code = instruction_code::negate;
        op.result_name = result;
        op.left_name = std::any_cast<std::string&&>(std::move(rhs.at(1)));
        instructions.push_back(op);
        return result;
      }
      case production_literal:
      {
        auto result = get_temporary();
        named_instruction op;
        op.code = instruction_code::assign_constant;
        op.result_name = result;
        op.constant = std::any_cast<double>(rhs.at(0));
        instructions.push_back(op);
        return result;
      }
    }
    return std::any();
  }
 private:
  std::string get_temporary()
  {
    return std::string("tmp") + std::to_string(++next_temporary);
  }
  struct live_range {
    std::string name;
    int when_written_to;
    int when_last_read;
    int register_assigned;
  };
  void update_live_ranges_for_read(std::size_t i, std::string const& name)
  {
    live_range* found_range = nullptr;
    for (auto& lr : live_ranges) {
      if (lr.name == name) {
        if (found_range == nullptr ||
            found_range->when_written_to < lr.when_written_to) {
          found_range = &lr;
        }
      }
    }
    if (found_range == nullptr) {
      live_range lr;
      lr.name = name;
      lr.when_written_to = -1;
      lr.when_last_read = int(i);
      live_ranges.push_back(lr);
    } else {
      found_range->when_last_read = int(i);
    }
  }
  void compute_live_ranges()
  {
    for (std::size_t i = 0; i < instructions.size(); ++i) {
      auto& op = instructions[i];
      if (!op.left_name.empty()) {
        update_live_ranges_for_read(i, op.left_name);
      }
      if (!op.right_name.empty()) {
        update_live_ranges_for_read(i, op.right_name);
      }
      live_range result_live_range;
      result_live_range.name = op.result_name;
      result_live_range.when_written_to = int(i);
      result_live_range.when_last_read = -2;
      live_ranges.push_back(result_live_range);
    }
    std::sort(live_ranges.begin(), live_ranges.end(),
        [] (live_range const& a, live_range const& b) {
          return a.when_written_to < b.when_written_to;
        });
    assign_registers();
  }
  void assign_registers()
  {
    std::vector<live_range*> active;
    std::vector<int> free_registers;
    for (auto& i : live_ranges) {
      for (std::size_t j = 0; j < active.size();) {
        if (active[j]->when_last_read > i.when_written_to) {
          ++j;
          continue;
        }
        free_registers.push_back(active[j]->register_assigned);
        active.erase(active.begin() + j);
      }
      if (free_registers.empty()) {
        free_registers.push_back(register_count++);
      }
      i.register_assigned = free_registers.back();
      free_registers.pop_back();
      active.insert(
          std::upper_bound(
            active.begin(),
            active.end(),
            &i,
            [] (live_range* a, live_range* b) {
              return a->when_last_read < b->when_last_read;
            }),
          &i);
    }
  }
  int next_temporary{0};
  std::vector<named_instruction> instructions;
  std::vector<live_range> live_ranges;
  int register_count{0};
};

}

int main() {
  auto l = rtc::build_language();
  auto rtp = parsegen::build_reader_tables(l);
  rtc::reader reader;
  reader.read_string(
      "double dist = coords[0] * coords[0] + coords[1] * coords[1] + coords[2] * coords[2];"
      ,
      "test");
}
