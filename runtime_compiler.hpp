#pragma once

#include <cstdint>
#include <cmath>
#include <map>
#include <string>
#include <vector>

#include "p3a_macros.hpp"
#include "p3a_dynamic_array.hpp"

namespace rtc {

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
  P3A_HOST P3A_DEVICE P3A_ALWAYS_INLINE
  inline void execute(double* registers) const;
};

P3A_HOST P3A_DEVICE P3A_ALWAYS_INLINE
inline void instruction::execute(double* registers) const {
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
        std::sqrt(registers[this->input_registers.left]);
      break;
    }
    case instruction_code::sin:
    {
      registers[this->result_register] =
        std::sin(registers[this->input_registers.left]);
      break;
    }
    case instruction_code::cos:
    {
      registers[this->result_register] =
        std::cos(registers[this->input_registers.left]);
      break;
    }
    case instruction_code::exp:
    {
      registers[this->result_register] =
        std::exp(registers[this->input_registers.left]);
      break;
    }
    case instruction_code::pow:
    {
      registers[this->result_register] =
        std::pow(
            registers[this->input_registers.left],
            registers[this->input_registers.right]);
      break;
    }
  }
}

class program_view {
 public:
  program_view(
      instruction const* instructions_in,
      int instruction_count_in)
    :instructions(instructions_in)
    ,instruction_count(instruction_count_in)
  {
  }
  P3A_HOST P3A_DEVICE P3A_ALWAYS_INLINE
  inline void execute(double* registers) const
  {
    for (int i = 0; i < instruction_count; ++i) {
      instructions[i].execute(registers);
    }
  }
 private:
  instruction const* instructions;
  int instruction_count;
};

class program {
 public:
  program(
      std::vector<instruction> const& instructions_in,
      std::map<std::string, int>&& input_registers_in,
      std::map<std::string, int>&& output_registers_in,
      int register_count_in)
    :m_input_registers(input_registers_in)
    ,m_output_registers(output_registers_in)
    ,m_register_count(register_count_in)
  {
    m_host_instructions.resize(instructions_in.size());
    p3a::copy(p3a::serial,
        instructions_in.cbegin(),
        instructions_in.cend(),
        m_host_instructions.begin());
    m_device_instructions.resize(m_host_instructions.size());
    p3a::copy(p3a::device,
        m_host_instructions.cbegin(),
        m_host_instructions.cend(),
        m_device_instructions.begin());
  }
  [[nodiscard]]
  int register_count() const { return m_register_count; }
  [[nodiscard]]
  int input_register(std::string const& name) const
  {
    return m_input_registers.at(name);
  }
  [[nodiscard]]
  int output_register(std::string const& name) const
  {
    return m_output_registers.at(name);
  }
  [[nodiscard]]
  program_view host_view() const
  {
    return program_view(m_host_instructions.data(), int(m_host_instructions.size()));
  }
  [[nodiscard]]
  program_view device_view() const
  {
    return program_view(m_device_instructions.data(), int(m_device_instructions.size()));
  }
 private:
  p3a::host_array<instruction> m_host_instructions;
  p3a::device_array<instruction> m_device_instructions;
  std::map<std::string, int> m_input_registers;
  std::map<std::string, int> m_output_registers;
  int m_register_count;
};

[[nodiscard]]
program compile(
    std::string const& source_code,
    std::vector<std::string> const& input_variables,
    std::vector<std::string> const& output_variables = {},
    std::string const& program_name = "runtime compiler input");

}
