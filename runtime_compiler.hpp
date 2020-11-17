#pragma once

#include <cstdint>
#include <cmath>
#include <map>
#include <string>
#include <vector>

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
  inline void execute(double* registers) const;
};

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
  {
  }
  inline void execute(double* registers)
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
      std::vector<instruction>&& instructions_in,
      std::map<std::string, int>&& input_registers_in,
      std::map<std::string, int>&& output_registers_in,
      int register_count_in)
    :m_instructions(instructions_in)
    ,m_input_registers(input_registers_in)
    ,m_output_registers(output_registers_in)
    ,m_register_count(register_count_in)
  {}
  int register_count() const { return m_register_count; }
  int input_register(std::string const& name) const
  {
    return m_input_registers.at(name);
  }
  int output_register(std::string const& name) const
  {
    return m_output_registers.at(name);
  }
  program_view view() const
  {
    return program_view(m_instructions.data(), int(m_instructions.size()));
  }
 private:
  std::vector<instruction> m_instructions;
  std::map<std::string, int> m_input_registers;
  std::map<std::string, int> m_output_registers;
  int m_register_count;
};

}
