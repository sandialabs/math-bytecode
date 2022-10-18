#include "math_bytecode.hpp"

#include "parsegen.hpp"

#include <algorithm>

#include <iostream>

namespace math_bytecode {

enum token : std::size_t {
  token_integer,
  token_floating_point,
  token_plus,
  token_minus,
  token_times,
  token_divide,
  token_raise,
  token_assign,
  token_open_parens,
  token_close_parens,
  token_open_array,
  token_close_array,
  token_double,
  token_const,
  token_reference,
  token_void,
  token_if,
  token_else,
  token_identifier,
  token_statement_end,
  token_argument_separator,
  token_open_block,
  token_close_block,
  token_logical_or,
  token_logical_and,
  token_logical_not,
  token_equal,
  token_not_equal,
  token_less,
  token_less_or_equal,
  token_greater,
  token_greater_or_equal,
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
  production_define_function,
  production_function_signature,
  production_first_parameter,
  production_next_parameter,
  production_input_scalar_parameter,
  production_output_scalar_parameter,
  production_array_parameter,
  production_if,
  production_if_else,
  production_if_header,
  production_block,
  production_variable,
  production_array_entry,
  production_first_declaration_specifier,
  production_next_declaration_specifier,
  production_const,
  production_double,
  production_sum_or_difference,
  production_product_or_quotient,
  production_decay_to_negation,
  production_decay_to_exponentiation,
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
  production_exponentiation,
  production_floating_point_literal,
  production_integer_literal,
  production_decay_to_or,
  production_decay_to_and,
  production_decay_to_not,
  production_decay_to_relational,
  production_logical_or,
  production_logical_and,
  production_logical_not,
  production_equal,
  production_not_equal,
  production_less,
  production_less_or_equal,
  production_greater,
  production_greater_or_equal,
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
    "floating_point", int_regex + "(\\.[0-9]*)?([eE][\\-\\+]?[0-9]+)?" + space_regex };
  l.tokens[token_plus] = {"plus", "\\+" + space_regex};
  l.tokens[token_minus] = {"minus", "\\-" + space_regex};
  l.tokens[token_times] = {"times", "\\*" + space_regex};
  l.tokens[token_divide] = {"divide", "/" + space_regex};
  l.tokens[token_raise] = {"raise", "\\^" + space_regex};
  l.tokens[token_assign] = {"assign", "=" + space_regex};
  l.tokens[token_open_parens] = {"open_parens", "\\(" + space_regex};
  l.tokens[token_close_parens] = {"close_parens", "\\)" + space_regex};
  l.tokens[token_open_array] = {"open_array", "\\[" + space_regex};
  l.tokens[token_close_array] = {"close_array", "\\]" + space_regex};
  l.tokens[token_double] = {"double", "double" + space_regex};
  l.tokens[token_const] = {"const", "const" + space_regex};
  l.tokens[token_reference] = {"reference", "&" + space_regex};
  l.tokens[token_void] = {"void", "void" + space_regex};
  l.tokens[token_if] = {"if", "if" + space_regex};
  l.tokens[token_else] = {"else", "else" + space_regex};
  l.tokens[token_identifier] = {"identifier", "[_A-Za-z][_A-Za-z0-9]*" + space_regex};
  l.tokens[token_statement_end] = {"statement_end", ";" + space_regex};
  l.tokens[token_argument_separator] = {"argument_separator", "," + space_regex};
  l.tokens[token_open_block] = {"open_block", "{" + space_regex};
  l.tokens[token_close_block] = {"close_block", "}" + space_regex};
  l.tokens[token_logical_or] = {"logical_or", "\\|\\|" + space_regex};
  l.tokens[token_logical_and] = {"logical_and", "&&" + space_regex};
  l.tokens[token_logical_not] = {"logical_not", "!" + space_regex};
  l.tokens[token_equal] = {"equal", "==" + space_regex};
  l.tokens[token_not_equal] = {"not_equal", "!=" + space_regex};
  l.tokens[token_less] = {"less", "<" + space_regex};
  l.tokens[token_less_or_equal] = {"less_or_equal", "<=" + space_regex};
  l.tokens[token_greater] = {"greater", ">" + space_regex};
  l.tokens[token_greater_or_equal] = {"greater_or_equal", ">=" + space_regex};
  l.productions.resize(production_count);
  l.productions[production_program] =
  {"program", {"function_definition"}};
  l.productions[production_first_statement] =
  {"statements", {"statement"}};
  l.productions[production_next_statement] =
  {"statements", {"statements", "statement"}};
  l.productions[production_assign] =
  {"statement", {"mutable", "assign", "immutable", "statement_end"}};
  l.productions[production_declare_assign] =
  {"statement", {"declaration_specifiers", "identifier", "assign", "immutable", "statement_end"}};
  l.productions[production_declare_scalar] =
  {"statement", {"declaration_specifiers", "identifier", "statement_end"}};
  l.productions[production_declare_array] =
  {"statement", {"declaration_specifiers", "identifier", "open_array", "integer", "close_array", "statement_end"}};
  l.productions[production_define_function] =
  {"function_definition", {"function_signature", "block"}};
  l.productions[production_function_signature] =
  {"function_signature", {"void", "identifier", "open_parens", "parameters", "close_parens"}};
  l.productions[production_first_parameter] =
  {"parameters", {"parameter"}};
  l.productions[production_next_parameter] =
  {"parameters", {"parameters", "argument_separator", "parameter"}};
  l.productions[production_input_scalar_parameter] =
  {"parameter", {"declaration_specifiers", "identifier"}};
  l.productions[production_output_scalar_parameter] =
  {"parameter", {"double", "reference", "identifier"}};
  l.productions[production_array_parameter] =
  {"parameter", {"declaration_specifiers", "identifier", "open_array", "integer", "close_array"}};
  l.productions[production_if] =
  {"statement", {"if_header", "block"}};
  l.productions[production_if_else] =
  {"statement", {"if_header", "block", "else", "block"}};
  l.productions[production_if_header] =
  {"if_header", {"if", "open_parens", "boolean_immutable", "close_parens"}};
  l.productions[production_block] =
  {"block", {"open_block", "statements", "close_block"}};
  l.productions[production_variable] =
  {"mutable", {"identifier"}};
  l.productions[production_array_entry] =
  {"mutable", {"identifier", "open_array", "integer", "close_array"}};
  l.productions[production_first_declaration_specifier] =
  {"declaration_specifiers", {"declaration_specifier"}};
  l.productions[production_next_declaration_specifier] =
  {"declaration_specifiers", {"declaration_specifiers", "declaration_specifier"}};
  l.productions[production_const] =
  {"declaration_specifier", {"const"}};
  l.productions[production_double] =
  {"declaration_specifier", {"double"}};
  l.productions[production_sum_or_difference] =
  {"immutable", {"sum_or_difference"}};
  l.productions[production_product_or_quotient] =
  {"sum_or_difference", {"product_or_quotient"}};
  l.productions[production_decay_to_negation] =
  {"product_or_quotient", {"negation"}};
  l.productions[production_decay_to_exponentiation] =
  {"negation", {"exponentiation"}};
  l.productions[production_decay_to_leaf] =
  {"exponentiation", {"leaf"}};
  l.productions[production_read] =
  {"leaf", {"mutable"}};
  l.productions[production_subexpression] =
  {"leaf", {"open_parens", "immutable", "close_parens"}};
  l.productions[production_unary_call] =
  {"leaf", {"identifier", "open_parens", "immutable", "close_parens"}};
  l.productions[production_binary_call] =
  {"leaf", {"identifier", "open_parens", "immutable", "argument_separator", "immutable", "close_parens"}};
  l.productions[production_sum] =
  {"sum_or_difference", {"sum_or_difference", "plus", "product_or_quotient"}};
  l.productions[production_difference] =
  {"sum_or_difference", {"sum_or_difference", "minus", "product_or_quotient"}};
  l.productions[production_product] =
  {"product_or_quotient", {"product_or_quotient", "times", "negation"}};
  l.productions[production_quotient] =
  {"product_or_quotient", {"product_or_quotient", "divide", "negation"}};
  l.productions[production_negation] =
  {"negation", {"minus", "exponentiation"}};
  // deliberately do not allow recursion on exponentiation since the rest
  // of the world doesn't agree on whether it is left or right associative
  l.productions[production_exponentiation] =
  {"exponentiation", {"leaf", "raise", "leaf"}};
  l.productions[production_floating_point_literal] =
  {"leaf", {"floating_point"}};
  l.productions[production_integer_literal] =
  {"leaf", {"integer"}};
  l.productions[production_decay_to_or] =
  {"boolean_immutable", {"logical_or_expression"}};
  l.productions[production_decay_to_and] =
  {"logical_or_expression", {"logical_and_expression"}};
  l.productions[production_decay_to_not] =
  {"logical_and_expression", {"logical_not_expression"}};
  l.productions[production_decay_to_relational] =
  {"logical_not_expression", {"relational_expression"}};
  l.productions[production_logical_or] =
  {"logical_or_expression", {"logical_or_expression", "logical_or", "logical_and_expression"}};
  l.productions[production_logical_and] =
  {"logical_and_expression", {"logical_and_expression", "logical_and", "relational_expression"}};
  l.productions[production_logical_not] =
  {"logical_not_expression", {"logical_not", "relational_expression"}};
  l.productions[production_equal] =
  {"relational_expression", {"immutable", "equal", "immutable"}};
  l.productions[production_not_equal] =
  {"relational_expression", {"immutable", "not_equal", "immutable"}};
  l.productions[production_less] =
  {"relational_expression", {"immutable", "less", "immutable"}};
  l.productions[production_less_or_equal] =
  {"relational_expression", {"immutable", "less_or_equal", "immutable"}};
  l.productions[production_greater] =
  {"relational_expression", {"immutable", "greater", "immutable"}};
  l.productions[production_greater_or_equal] =
  {"relational_expression", {"immutable", "greater_or_equal", "immutable"}};
  return l;
}

static inline std::string remove_leading_space(std::string s) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), [] (char ch) {
    return !std::isspace(ch);
  }));
  return s;
}

static inline std::string remove_trailing_space(std::string s) {
  s.erase(std::find_if(s.rbegin(), s.rend(), [] (char ch) {
    return !std::isspace(ch);
  }).base(), s.end());
  return s;
}

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
      break;
    }
    case instruction_code::sin:
    {
      s << op.result_name << " = sin("
        << op.left_name << ")\n";
      break;
    }
    case instruction_code::cos:
    {
      s << op.result_name << " = cos("
        << op.left_name << ")\n";
      break;
    }
    case instruction_code::exp:
    {
      s << op.result_name << " = exp("
        << op.left_name << ")\n";
      break;
    }
    case instruction_code::pow:
    {
      s << op.result_name << " = pow("
        << op.left_name << ", "
        << op.right_name << ")\n";
      break;
    }
    case instruction_code::conditional_copy:
    {
      s << "if (" << op.left_name << ") " << op.result_name << " = " << op.right_name << "\n";
      break;
    }
    case instruction_code::logical_or:
    {
      s << op.result_name << " = "
        << op.left_name << " || "
        << op.right_name << "\n";
      break;
    }
    case instruction_code::logical_and:
    {
      s << op.result_name << " = "
        << op.left_name << " && "
        << op.right_name << "\n";
      break;
    }
    case instruction_code::logical_not:
    {
      s << op.result_name << " = !"
        << op.left_name << "\n";
      break;
    }
    case instruction_code::equal:
    {
      s << op.result_name << " = "
        << op.left_name << " == "
        << op.right_name << "\n";
      break;
    }
    case instruction_code::not_equal:
    {
      s << op.result_name << " = "
        << op.left_name << " != "
        << op.right_name << "\n";
      break;
    }
    case instruction_code::less:
    {
      s << op.result_name << " = "
        << op.left_name << " < "
        << op.right_name << "\n";
      break;
    }
    case instruction_code::less_or_equal:
    {
      s << op.result_name << " = "
        << op.left_name << " <= "
        << op.right_name << "\n";
      break;
    }
    case instruction_code::greater:
    {
      s << op.result_name << " = "
        << op.left_name << " > "
        << op.right_name << "\n";
      break;
    }
    case instruction_code::greater_or_equal:
    {
      s << op.result_name << " = "
        << op.left_name << " >= "
        << op.right_name << "\n";
      break;
    }
  }
  return s;
}

std::ostream& operator<<(
    std::ostream& s, instruction const& op)
{
  switch (op.code) {
    case instruction_code::copy:
    {
      s << "$" << op.result_register << " = $" << op.input_registers.left << '\n';
      break;
    }
    case instruction_code::add:
    {
      s << "$" << op.result_register << " = $"
        << op.input_registers.left << " + $"
        << op.input_registers.right << '\n';
      break;
    }
    case instruction_code::subtract:
    {
      s << "$" << op.result_register << " = $"
        << op.input_registers.left << " - $"
        << op.input_registers.right << '\n';
      break;
    }
    case instruction_code::multiply:
    {
      s << "$" << op.result_register << " = $"
        << op.input_registers.left << " * $"
        << op.input_registers.right << '\n';
      break;
    }
    case instruction_code::divide:
    {
      s << "$" << op.result_register << " = $"
        << op.input_registers.left << " / $"
        << op.input_registers.right << '\n';
      break;
    }
    case instruction_code::negate:
    {
      s << "$" << op.result_register << " = -$"
        << op.input_registers.left << '\n';
      break;
    }
    case instruction_code::assign_constant:
    {
      s << "$" << op.result_register << " = "
        << op.constant << '\n';
      break;
    }
    case instruction_code::sqrt:
    {
      s << "$" << op.result_register << " = sqrt($"
        << op.input_registers.left << ")\n";
      break;
    }
    case instruction_code::sin:
    {
      s << "$" << op.result_register << " = sin($"
        << op.input_registers.left << ")\n";
      break;
    }
    case instruction_code::cos:
    {
      s << "$" << op.result_register << " = cos($"
        << op.input_registers.left << ")\n";
      break;
    }
    case instruction_code::exp:
    {
      s << "$" << op.result_register << " = exp($"
        << op.input_registers.left << ")\n";
      break;
    }
    case instruction_code::pow:
    {
      s << "$" << op.result_register << " = pow($"
        << op.input_registers.left << ", $"
        << op.input_registers.right << ")\n";
      break;
    }
    case instruction_code::conditional_copy:
    {
      s << "if ($" << op.input_registers.left << ") $"
        << op.result_register << " = $"
        << op.input_registers.right << "\n";
      break;
    }
    case instruction_code::logical_or:
    {
      s << "$" << op.result_register << " = $"
        << op.input_registers.left << " || $"
        << op.input_registers.right << "\n";
      break;
    }
    case instruction_code::logical_and:
    {
      s << "$" << op.result_register << " = $"
        << op.input_registers.left << " && $"
        << op.input_registers.right << "\n";
      break;
    }
    case instruction_code::logical_not:
    {
      s << "$" << op.result_register << " = !$"
        << op.input_registers.left << "\n";
      break;
    }
    case instruction_code::equal:
    {
      s << "$" << op.result_register << " = $"
        << op.input_registers.left << " == $"
        << op.input_registers.right << "\n";
      break;
    }
    case instruction_code::not_equal:
    {
      s << "$" << op.result_register << " = $"
        << op.input_registers.left << " != $"
        << op.input_registers.right << "\n";
      break;
    }
    case instruction_code::less:
    {
      s << "$" << op.result_register << " = $"
        << op.input_registers.left << " < $"
        << op.input_registers.right << "\n";
      break;
    }
    case instruction_code::less_or_equal:
    {
      s << "$" << op.result_register << " = $"
        << op.input_registers.left << " <= $"
        << op.input_registers.right << "\n";
      break;
    }
    case instruction_code::greater:
    {
      s << "$" << op.result_register << " = $"
        << op.input_registers.left << " > $"
        << op.input_registers.right << "\n";
      break;
    }
    case instruction_code::greater_or_equal:
    {
      s << "$" << op.result_register << " = $"
        << op.input_registers.left << " >= $"
        << op.input_registers.right << "\n";
      break;
    }
  }
  return s;
}

instruction_code binary_operator_code(int p)
{
  switch (p) {
    case production_sum: return instruction_code::add;
    case production_difference: return instruction_code::subtract;
    case production_product: return instruction_code::multiply;
    case production_quotient: return instruction_code::divide;
    case production_exponentiation: return instruction_code::pow;
    case production_logical_or: return instruction_code::logical_or;
    case production_logical_and: return instruction_code::logical_and;
    case production_equal: return instruction_code::equal;
    case production_not_equal: return instruction_code::not_equal;
    case production_less: return instruction_code::less;
    case production_less_or_equal: return instruction_code::less_or_equal;
    case production_greater: return instruction_code::greater;
    case production_greater_or_equal: return instruction_code::greater_or_equal;
  }
  throw parsegen::parse_error("BUG: unexpected binary production");
}

class parser : public parsegen::parser
{
 public:
  parser(bool verbose)
    :parsegen::parser(
        parsegen::build_parser_tables(
          math_bytecode::build_language()))
    ,is_verbose(verbose)
  {
  }
  virtual std::any shift(int token, std::string& text) override
  {
    switch (token) {
      case token_identifier: return remove_trailing_space(std::move(text));
      case token_integer: return std::stoi(text);
      case token_floating_point: return std::stod(text);
      case token_else:
      {
        named_instruction op;
        op.code = instruction_code::logical_not;
        op.result_name = condition_name;
        op.left_name = condition_name;
        named_instructions.push_back(op);
        break;
      }
    }
    return std::any();
  }
  virtual std::any reduce(
      int production, std::vector<std::any>& rhs) override
  {
    switch (production) {
      case production_program:
      {
        if (is_verbose) {
          for (std::size_t i = 0; i < named_instructions.size(); ++i) {
            std::cout << i << ": " << named_instructions[i];
          }
        }
        compute_live_ranges();
        if (is_verbose) {
          for (auto& lr : live_ranges) {
            std::cout << lr.name << " at register " << lr.register_assigned
              << " from " << lr.when_written_to << " to " << lr.when_last_read << '\n';
          }
        }
        generate_instructions();
        if (is_verbose) {
          for (std::size_t i = 0; i < instructions.size(); ++i) {
            std::cout << i << ": " << instructions[i];
          }
        }
        lookup_registers();
        if (is_verbose) {
          for (std::size_t i = 0; i < input_registers.size(); ++i) {
            std::cout << "input variable " << input_variable_names[i] << " at register " << input_registers[i] << '\n';
          }
          for (std::size_t i = 0; i < output_registers.size(); ++i) {
            std::cout << "output variable " << output_variable_names[i] << " at register " << output_registers[i] << '\n';
          }
        }
        break;
      }
      case production_input_scalar_parameter:
      {
        input_variable_names.push_back(std::any_cast<std::string&&>(std::move(rhs.at(1))));
        break;
      }
      case production_output_scalar_parameter:
      {
        output_variable_names.push_back(std::any_cast<std::string&&>(std::move(rhs.at(2))));
        break;
      }
      case production_array_parameter:
      {
        bool const is_const = std::any_cast<bool>(rhs.at(0));
        int const n = std::any_cast<int>(rhs.at(3));
        std::string const name(std::any_cast<std::string&&>(std::move(rhs.at(1))));
        for (int i = 0; i < n; ++i) {
          if (is_const) {
            input_variable_names.push_back(name + "[" + std::to_string(i) + "]");
          } else {
            output_variable_names.push_back(name + "[" + std::to_string(i) + "]");
          }
        }
        break;
      }
      case production_assign:
      {
        handle_assign(
          std::any_cast<std::string&&>(std::move(rhs.at(0))),
          std::any_cast<std::string&&>(std::move(rhs.at(2))));
        break;
      }
      case production_declare_assign:
      {
        handle_assign(
          std::any_cast<std::string&&>(std::move(rhs.at(1))),
          std::any_cast<std::string&&>(std::move(rhs.at(3))));
        break;
      }
      case production_if:
      case production_if_else:
      {
        is_inside_conditional = false;
        break;
      }
      case production_if_header:
      {
        if (is_inside_conditional) {
          throw parsegen::parse_error(
              "nested if/else blocks are not supported");
        }
        condition_name =
          std::any_cast<std::string&&>(std::move(rhs.at(2)));
        is_inside_conditional = true;
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
      case production_decay_to_exponentiation:
      case production_decay_to_leaf:
      case production_decay_to_or:
      case production_decay_to_and:
      case production_decay_to_not:
      case production_decay_to_relational:
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
        } else if (function_name == "sin") {
          op.code = instruction_code::sin;
        } else if (function_name == "cos") {
          op.code = instruction_code::cos;
        } else if (function_name == "exp") {
          op.code = instruction_code::exp;
        } else {
          throw parsegen::parse_error("unknown unary function name");
        }
        op.left_name =
          std::any_cast<std::string&&>(
              std::move(rhs.at(2)));
        named_instructions.push_back(op);
        return result;
      }
      case production_binary_call:
      {
        auto result = get_temporary();
        auto function_name =
          remove_trailing_space(
              std::any_cast<std::string&&>(
                std::move(rhs.at(0))));
        named_instruction op;
        op.result_name = result;
        if (function_name == "pow") {
          op.code = instruction_code::pow;
        } else {
          throw parsegen::parse_error("unknown binary function name");
        }
        op.left_name =
          std::any_cast<std::string&&>(
              std::move(rhs.at(2)));
        op.right_name =
          std::any_cast<std::string&&>(
              std::move(rhs.at(4)));
        named_instructions.push_back(op);
        return result;
      }
      case production_sum:
      case production_difference:
      case production_product:
      case production_quotient:
      case production_exponentiation:
      case production_logical_or:
      case production_logical_and:
      case production_equal:
      case production_not_equal:
      case production_less:
      case production_less_or_equal:
      case production_greater:
      case production_greater_or_equal:
      {
        auto result = get_temporary();
        named_instruction op;
        op.code = binary_operator_code(production);
        op.result_name = result;
        op.left_name = std::any_cast<std::string&&>(std::move(rhs.at(0)));
        op.right_name = std::any_cast<std::string&&>(std::move(rhs.at(2)));
        named_instructions.push_back(op);
        return result;
      }
      case production_negation:
      {
        auto result = get_temporary();
        named_instruction op;
        op.code = instruction_code::negate;
        op.result_name = result;
        op.left_name = std::any_cast<std::string&&>(std::move(rhs.at(1)));
        named_instructions.push_back(op);
        return result;
      }
      case production_logical_not:
      {
        auto result = get_temporary();
        named_instruction op;
        op.code = instruction_code::logical_not;
        op.result_name = result;
        op.left_name = std::any_cast<std::string&&>(std::move(rhs.at(1)));
        named_instructions.push_back(op);
        return result;
      }
      case production_floating_point_literal:
      {
        auto result = get_temporary();
        named_instruction op;
        op.code = instruction_code::assign_constant;
        op.result_name = result;
        op.constant = std::any_cast<double>(rhs.at(0));
        named_instructions.push_back(op);
        return result;
      }
      case production_integer_literal:
      {
        auto result = get_temporary();
        named_instruction op;
        op.code = instruction_code::assign_constant;
        op.result_name = result;
        op.constant = double(std::any_cast<int>(rhs.at(0)));
        named_instructions.push_back(op);
        return result;
      }
      case production_first_declaration_specifier:
      {
        return rhs.at(0);
      }
      case production_next_declaration_specifier:
      {
        return std::any_cast<bool>(rhs.at(0)) || std::any_cast<bool>(rhs.at(1));
      }
      case production_const:
      {
        return true;
      }
      case production_double:
      {
        return false;
      }
    }
    return std::any();
  }
  host_function get_function()
  {
    return host_function(
        std::move(instructions),
        std::move(input_registers),
        std::move(output_registers),
        register_count);
  }
 private:
  std::string get_temporary()
  {
    return std::string("tmp") + std::to_string(++next_temporary);
  }
  void handle_assign(std::string const& destination, std::string const& source)
  {
    named_instruction op;
    if (is_inside_conditional) {
      op.code = instruction_code::conditional_copy;
      op.result_name = destination;
      op.left_name = condition_name;
      op.right_name = source;
    } else {
      op.code = instruction_code::copy;
      op.result_name = destination;
      op.left_name = source;
    }
    named_instructions.push_back(op);
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
    for (std::size_t i = 0; i < named_instructions.size(); ++i) {
      auto& op = named_instructions[i];
      if (!op.left_name.empty()) {
        update_live_ranges_for_read(i, op.left_name);
      }
      if (!op.right_name.empty()) {
        update_live_ranges_for_read(i, op.right_name);
      }
      bool is_conditional_assign_to_existing = false;
      if (op.code == instruction_code::conditional_copy) {
        for (auto& lr : live_ranges) {
          if (lr.name == op.result_name) {
            is_conditional_assign_to_existing = true;
          }
        }
      }
      if (is_conditional_assign_to_existing) continue;
      live_range result_live_range;
      result_live_range.name = op.result_name;
      result_live_range.when_written_to = int(i);
      result_live_range.when_last_read = -2;
      for (auto& output_variable_name : output_variable_names) {
        if (op.result_name == output_variable_name) {
          result_live_range.when_last_read = int(named_instructions.size());
          break;
        }
      }
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
        if (i.when_written_to >= 0 && i.when_written_to < int(named_instructions.size())
            && named_instructions.at(i.when_written_to).code == instruction_code::conditional_copy) {
          if (active[j]->when_last_read == i.when_written_to) {
            ++j;
            continue;
          }
        }
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
  void generate_instructions()
  {
    instructions.resize(named_instructions.size());
    for (std::size_t i = 0; i < instructions.size(); ++i) {
      instructions[i].code = named_instructions[i].code;
      if (named_instructions[i].code == instruction_code::assign_constant) {
        instructions[i].constant = named_instructions[i].constant;
      }
    }
    for (auto& lr : live_ranges) {
      auto first = std::size_t(std::max(lr.when_written_to, 0));
      auto last = std::size_t(std::max(lr.when_last_read + 1, lr.when_written_to));
      last = std::min(last, named_instructions.size());
      for (std::size_t i = first; i < last; ++i) {
        if (named_instructions[i].result_name == lr.name) {
          instructions[i].result_register = lr.register_assigned;
        }
        if (named_instructions[i].left_name == lr.name) {
          instructions[i].input_registers.left = lr.register_assigned;
        }
        if (named_instructions[i].right_name == lr.name) {
          instructions[i].input_registers.right = lr.register_assigned;
        }
      }
    }
  }
  int get_input_register(std::string const& name) const
  {
    for (auto& lr : live_ranges) {
      if (lr.name == name && lr.when_written_to == -1) {
        return lr.register_assigned;
      }
    }
    return -1;
  }
  int get_output_register(std::string const& name) const
  {
    for (auto& lr : live_ranges) {
      if (lr.name == name && lr.when_last_read == int(instructions.size())) {
        return lr.register_assigned;
      }
    }
    throw parsegen::parse_error(
        "function does not set required output variable" +
        name);
  }
  void lookup_registers()
  {
    for (auto& input_name : input_variable_names) {
      input_registers.push_back(get_input_register(input_name));
    }
    for (auto& output_name : output_variable_names) {
      output_registers.push_back(get_output_register(output_name));
    }
  }
  int next_temporary{0};
  std::vector<named_instruction> named_instructions;
  std::vector<instruction> instructions;
  std::vector<live_range> live_ranges;
  int register_count{0};
  std::vector<std::string> input_variable_names;
  std::vector<std::string> output_variable_names;
  std::vector<int> input_registers;
  std::vector<int> output_registers;
  std::string condition_name;
  bool is_inside_conditional{false};
  bool is_verbose;
};

host_function compile(
    std::string const& source_code,
    bool verbose)
{
  math_bytecode::parser parser(verbose);
  parser.parse_string(
    math_bytecode::remove_leading_space(source_code),
    "runtime math function");
  return parser.get_function();
}

}
