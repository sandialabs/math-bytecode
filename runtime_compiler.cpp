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

}

int main() {
  auto l = rtc::build_language();
  auto rtp = parsegen::build_reader_tables(l);
  parsegen::debug_reader reader(rtp, std::cout);
  reader.read_string("field[0] = coords[0] * coords[0];", "test");
}
