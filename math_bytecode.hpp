#pragma once

#include <cstdint>
#include <cmath>
#include <string>
#include <vector>

#include "p3a_macros.hpp"
#include "p3a_dynamic_array.hpp"
#include "p3a_quantity.hpp"

namespace math_bytecode {

enum class instruction_code : std::int32_t {
  copy,
  add,
  subtract,
  multiply,
  divide,
  negate,
  assign_constant,
  sqrt,
  sin,
  cos,
  exp,
  pow,
  conditional_copy,
  logical_or,
  logical_and,
  logical_not,
  equal,
  not_equal,
  less,
  less_or_equal,
  greater,
  greater_or_equal
};

class instruction {
 public:
  std::int32_t result_register;
  instruction_code code;
  union {
    struct {
      std::int32_t left;
      std::int32_t right;
    } input_registers;
    double constant;
  };
  template <class ScalarType>
  P3A_HOST P3A_DEVICE P3A_ALWAYS_INLINE
  inline void execute(ScalarType* registers) const;
};

template <class ScalarType>
P3A_HOST P3A_DEVICE P3A_ALWAYS_INLINE
inline void instruction::execute(ScalarType* registers) const {
  switch (this->code) {
    case instruction_code::copy:
    {
      registers[this->result_register] =
        registers[this->input_registers.left];
      break;
    }
    case instruction_code::add:
    {
      registers[this->result_register] =
        registers[this->input_registers.left] +
        registers[this->input_registers.right];
      break;
    }
    case instruction_code::subtract:
    {
      registers[this->result_register] =
        registers[this->input_registers.left] -
        registers[this->input_registers.right];
      break;
    }
    case instruction_code::multiply:
    {
      registers[this->result_register] =
        registers[this->input_registers.left] *
        registers[this->input_registers.right];
      break;
    }
    case instruction_code::divide:
    {
      registers[this->result_register] =
        registers[this->input_registers.left] /
        registers[this->input_registers.right];
      break;
    }
    case instruction_code::negate:
    {
      registers[this->result_register] =
        -registers[this->input_registers.left];
      break;
    }
    case instruction_code::assign_constant:
    {
      registers[this->result_register] = this->constant;
      break;
    }
    case instruction_code::sqrt:
    {
      registers[this->result_register] =
        p3a::square_root(registers[this->input_registers.left]);
      break;
    }
    case instruction_code::sin:
    {
      registers[this->result_register] =
        p3a::sine(registers[this->input_registers.left]);
      break;
    }
    case instruction_code::cos:
    {
      registers[this->result_register] =
        p3a::cosine(registers[this->input_registers.left]);
      break;
    }
    case instruction_code::exp:
    {
      registers[this->result_register] =
        p3a::natural_exponential(registers[this->input_registers.left]);
      break;
    }
    case instruction_code::pow:
    {
      registers[this->result_register] =
        p3a::exponentiate(
            registers[this->input_registers.left],
            registers[this->input_registers.right]);
      break;
    }
    case instruction_code::conditional_copy:
    {
      registers[this->result_register] =
        p3a::condition(
            registers[this->input_registers.left] != ScalarType(0.0),
            registers[this->input_registers.right],
            registers[this->result_register]);
      break;
    }
    case instruction_code::logical_or:
    {
      registers[this->result_register] =
        p3a::condition(
            (registers[this->input_registers.left] != ScalarType(0.0)) ||
            (registers[this->input_registers.right] != ScalarType(0.0)),
            ScalarType(1.0),
            ScalarType(0.0));
      break;
    }
    case instruction_code::logical_and:
    {
      registers[this->result_register] =
        p3a::condition(
            (registers[this->input_registers.left] != ScalarType(0.0)) &&
            (registers[this->input_registers.right] != ScalarType(0.0)),
            ScalarType(1.0),
            ScalarType(0.0));
      break;
    }
    case instruction_code::logical_not:
    {
      registers[this->result_register] =
        p3a::condition(
            registers[this->input_registers.left] != ScalarType(0.0),
            ScalarType(0.0),
            ScalarType(1.0));
      break;
    }
    case instruction_code::equal:
    {
      registers[this->result_register] =
        p3a::condition(
            registers[this->input_registers.left] ==
            registers[this->input_registers.right],
            ScalarType(1.0),
            ScalarType(0.0));
      break;
    }
    case instruction_code::not_equal:
    {
      registers[this->result_register] =
        p3a::condition(
            registers[this->input_registers.left] !=
            registers[this->input_registers.right],
            ScalarType(1.0),
            ScalarType(0.0));
      break;
    }
    case instruction_code::less:
    {
      registers[this->result_register] =
        p3a::condition(
            registers[this->input_registers.left] <
            registers[this->input_registers.right],
            ScalarType(1.0),
            ScalarType(0.0));
      break;
    }
    case instruction_code::less_or_equal:
    {
      registers[this->result_register] =
        p3a::condition(
            registers[this->input_registers.left] <=
            registers[this->input_registers.right],
            ScalarType(1.0),
            ScalarType(0.0));
      break;
    }
    case instruction_code::greater:
    {
      registers[this->result_register] =
        p3a::condition(
            registers[this->input_registers.left] >
            registers[this->input_registers.right],
            ScalarType(1.0),
            ScalarType(0.0));
      break;
    }
    case instruction_code::greater_or_equal:
    {
      registers[this->result_register] =
        p3a::condition(
            registers[this->input_registers.left] >=
            registers[this->input_registers.right],
            ScalarType(1.0),
            ScalarType(0.0));
      break;
    }
  }
}

class executable_function {
 public:
  P3A_ALWAYS_INLINE executable_function() = default;
  executable_function(
      instruction const* instructions_in,
      int instruction_count_in,
      int const* input_registers_in,
      int,
      int const* output_registers_in,
      int)
    :instructions(instructions_in)
    ,instruction_count(instruction_count_in)
    ,input_registers(input_registers_in)
    ,output_registers(output_registers_in)
  {
  }
  template <class ScalarType>
  P3A_HOST P3A_DEVICE P3A_ALWAYS_INLINE
  inline void execute(ScalarType* registers) const
  {
    for (int i = 0; i < instruction_count; ++i) {
      instructions[i].execute(registers);
    }
  }
  template <class ScalarType, class ... ArgumentTypes>
  P3A_HOST P3A_DEVICE P3A_ALWAYS_INLINE
  inline void operator()(
      ScalarType* registers,
      ArgumentTypes&& ... arguments) const
  {
    handle_input_arguments(registers, 0, std::forward<ArgumentTypes>(arguments) ...);
    execute(registers);
    handle_output_arguments(registers, 0, std::forward<ArgumentTypes>(arguments) ...);
  }
  template <class ScalarType, class FirstArgumentType, class ... NextArgumentTypes>
  P3A_HOST P3A_DEVICE P3A_ALWAYS_INLINE
  inline void handle_input_arguments(
      ScalarType* registers,
      int input_scalar_count,
      FirstArgumentType&& first_argument,
      NextArgumentTypes&& ... next_arguments) const
  {
    input_scalar_count = handle_input_argument(
        registers, input_scalar_count, std::forward<FirstArgumentType>(first_argument));
    handle_input_arguments(registers, input_scalar_count, std::forward<NextArgumentTypes>(next_arguments) ...);
  }
  template <class ScalarType, class LastArgumentType>
  P3A_HOST P3A_DEVICE P3A_ALWAYS_INLINE
  inline void handle_input_arguments(
      ScalarType* registers,
      int input_scalar_count,
      LastArgumentType&& last_argument) const
  {
    handle_input_argument(registers, input_scalar_count, std::forward<LastArgumentType>(last_argument));
  }
  template <class ScalarType, class NotInputType>
  P3A_HOST P3A_DEVICE P3A_ALWAYS_INLINE
  inline int handle_input_argument(
      ScalarType* registers,
      int input_scalar_count,
      NotInputType&& argument) const
  {
    return input_scalar_count;
  }
  template <class ScalarType>
  P3A_HOST P3A_DEVICE P3A_ALWAYS_INLINE
  inline int handle_input_argument(
      ScalarType* registers,
      int input_scalar_count,
      const ScalarType& argument) const
  {
    int const input_register = input_registers[input_scalar_count];
    if (input_register >= 0) {
      registers[input_register] = argument;
    }
    return input_scalar_count + 1;
  }
  template <class ScalarType, std::size_t N>
  P3A_HOST P3A_DEVICE P3A_ALWAYS_INLINE
  inline int handle_input_argument(
      ScalarType* registers,
      int input_scalar_count,
      const ScalarType (&argument) [N]) const
  {
    for (std::size_t i = 0; i < N; ++i) {
      int const input_register = input_registers[input_scalar_count];
      if (input_register >= 0) {
        registers[input_register] = argument[i];
      }
      ++input_scalar_count;
    }
    return input_scalar_count;
  }
  template <class ScalarType, std::size_t N>
  P3A_HOST P3A_DEVICE P3A_ALWAYS_INLINE
  inline int handle_input_argument(
      ScalarType* registers,
      int input_scalar_count,
      ScalarType (&argument) [N]) const
  {
    return input_scalar_count;
  }
  template <class ScalarType, class Unit, class Origin>
  P3A_HOST P3A_DEVICE P3A_ALWAYS_INLINE
  inline int handle_input_argument(
      ScalarType* registers,
      int input_scalar_count,
      const p3a::quantity<Unit, ScalarType, Origin>& argument) const
  {
    return handle_input_argument(registers, input_scalar_count, argument.value());
  }
  template <class ScalarType, class Unit, class Origin>
  P3A_HOST P3A_DEVICE P3A_ALWAYS_INLINE
  inline int handle_input_argument(
      ScalarType* registers,
      int input_scalar_count,
      p3a::vector3<p3a::quantity<Unit, ScalarType, Origin>> const& argument) const
  {
    ScalarType const values[3] = {argument.x().value(), argument.y().value(), argument.z().value()};
    return handle_input_argument(registers, input_scalar_count, values);
  }
  template <class ScalarType, class Unit, class Origin>
  P3A_HOST P3A_DEVICE P3A_ALWAYS_INLINE
  inline int handle_input_argument(
      ScalarType* registers,
      int input_scalar_count,
      p3a::vector3<p3a::quantity<Unit, ScalarType, Origin>>& argument) const
  {
    return input_scalar_count;
  }
  template <class ScalarType, class FirstArgumentType, class ... NextArgumentTypes>
  P3A_HOST P3A_DEVICE P3A_ALWAYS_INLINE
  inline void handle_output_arguments(
      ScalarType* registers,
      int output_scalar_count,
      FirstArgumentType&& first_argument,
      NextArgumentTypes&& ... next_arguments) const
  {
    output_scalar_count = handle_output_argument(
        registers, output_scalar_count, std::forward<FirstArgumentType>(first_argument));
    handle_output_arguments(registers, output_scalar_count, std::forward<NextArgumentTypes>(next_arguments) ...);
  }
  template <class ScalarType, class LastArgumentType>
  P3A_HOST P3A_DEVICE P3A_ALWAYS_INLINE
  inline void handle_output_arguments(
      ScalarType* registers,
      int output_scalar_count,
      LastArgumentType&& last_argument) const
  {
    handle_output_argument(registers, output_scalar_count, std::forward<LastArgumentType>(last_argument));
  }
  template <class ScalarType, class NotOutputType>
  P3A_HOST P3A_DEVICE P3A_ALWAYS_INLINE
  inline int handle_output_argument(
      ScalarType* registers,
      int output_scalar_count,
      NotOutputType&& argument) const
  {
    return output_scalar_count;
  }
  template <class ScalarType>
  P3A_HOST P3A_DEVICE P3A_ALWAYS_INLINE
  inline int handle_output_argument(
      ScalarType* registers,
      int output_scalar_count,
      ScalarType& argument) const
  {
    int const output_register = output_registers[output_scalar_count];
    argument = registers[output_register];
    return output_scalar_count + 1;
  }
  template <class ScalarType, std::size_t N>
  P3A_HOST P3A_DEVICE P3A_ALWAYS_INLINE
  inline int handle_output_argument(
      ScalarType* registers,
      int output_scalar_count,
      ScalarType (&argument) [N]) const
  {
    for (std::size_t i = 0; i < N; ++i) {
      int const output_register = output_registers[output_scalar_count];
      argument[i] = registers[output_register];
      ++output_scalar_count;
    }
    return output_scalar_count;
  }
  template <class ScalarType, class Unit, class Origin>
  P3A_HOST P3A_DEVICE P3A_ALWAYS_INLINE
  inline int handle_output_argument(
      ScalarType* registers,
      int output_scalar_count,
      p3a::quantity<Unit, ScalarType, Origin>& argument) const
  {
    return handle_output_argument(registers, output_scalar_count, argument.value());
  }
  template <class ScalarType, class Unit, class Origin>
  P3A_HOST P3A_DEVICE P3A_ALWAYS_INLINE
  inline int handle_output_argument(
      ScalarType* registers,
      int output_scalar_count,
      p3a::vector3<p3a::quantity<Unit, ScalarType, Origin>>& argument) const
  {
    ScalarType values[3];
    output_scalar_count = handle_output_argument(registers, output_scalar_count, values);
    argument.x() = p3a::quantity<Unit, ScalarType, Origin>(values[0]);
    argument.y() = p3a::quantity<Unit, ScalarType, Origin>(values[1]);
    argument.z() = p3a::quantity<Unit, ScalarType, Origin>(values[2]);
    return output_scalar_count;
  }
 private:
  instruction const* instructions;
  int instruction_count;
  int const* input_registers;
  int const* output_registers;
};

template <
  class Allocator,
  class ExecutionPolicy>
class compiled_function {
 public:
  using instructions_type = p3a::dynamic_array<::math_bytecode::instruction, Allocator, ExecutionPolicy>;
  using registers_type = p3a::dynamic_array<int, typename Allocator::template rebind<int>::other, ExecutionPolicy>;
  compiled_function(
      std::vector<instruction> const& instructions_in,
      std::vector<int> const& input_registers_in,
      std::vector<int> const& output_registers_in,
      int register_count_in)
    :m_register_count(register_count_in)
  {
    m_instructions.resize(instructions_in.size());
    p3a::copy(m_instructions.get_execution_policy(),
        instructions_in.cbegin(),
        instructions_in.cend(),
        m_instructions.begin());
    m_input_registers.resize(input_registers_in.size());
    p3a::copy(m_input_registers.get_execution_policy(),
        input_registers_in.cbegin(),
        input_registers_in.cend(),
        m_input_registers.begin());
    m_output_registers.resize(output_registers_in.size());
    p3a::copy(m_output_registers.get_execution_policy(),
        output_registers_in.cbegin(),
        output_registers_in.cend(),
        m_output_registers.begin());
  }
  template <class Allocator2, class ExecutionPolicy2>
  explicit
  compiled_function(compiled_function<Allocator2, ExecutionPolicy2> const& other)
    :m_register_count(other.register_count())
  {
    m_instructions.resize(other.instructions().size());
    p3a::copy(p3a::device,
        other.instructions().cbegin(),
        other.instructions().cend(),
        m_instructions.begin());
    m_input_registers.resize(other.input_registers().size());
    p3a::copy(m_input_registers.get_execution_policy(),
        other.input_registers().cbegin(),
        other.input_registers().cend(),
        m_input_registers.begin());
    m_output_registers.resize(other.output_registers().size());
    p3a::copy(m_output_registers.get_execution_policy(),
        other.output_registers().cbegin(),
        other.output_registers().cend(),
        m_output_registers.begin());
  }
  [[nodiscard]]
  executable_function executable() const
  {
    return executable_function(
        m_instructions.data(),
        int(m_instructions.size()),
        m_input_registers.data(),
        int(m_input_registers.size()),
        m_output_registers.data(),
        int(m_output_registers.size()));
  }
  [[nodiscard]]
  instructions_type const&
  instructions() const { return m_instructions; }
  [[nodiscard]]
  registers_type const&
  input_registers() const { return m_input_registers; }
  [[nodiscard]]
  registers_type const&
  output_registers() const { return m_output_registers; }
  [[nodiscard]]
  int register_count() const { return m_register_count; }
 private:
  instructions_type m_instructions;
  registers_type m_input_registers;
  registers_type m_output_registers;
  int m_register_count;
};

using host_function = compiled_function<p3a::allocator<instruction>, p3a::serial_local_execution>;
using device_function = compiled_function<p3a::device_allocator<instruction>, p3a::device_execution>;

[[nodiscard]]
host_function compile(std::string const& source_code, bool verbose = false);

}
