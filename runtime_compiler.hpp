#pragma once

#include <cstdint>
#include <cmath>

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
};

inline void execute(instruction const& op, double* registers) {
  switch (op.code) {
    case instruction_code::copy:
    {
      registers[op.result_register] =
        registers[op.input_registers.left];
      break;
    }
    case instruction_code::add:
    {
      registers[op.result_register] =
        registers[op.input_registers.left] +
        registers[op.input_registers.right];
      break;
    }
    case instruction_code::subtract:
    {
      registers[op.result_register] =
        registers[op.input_registers.left] -
        registers[op.input_registers.right];
      break;
    }
    case instruction_code::multiply:
    {
      registers[op.result_register] =
        registers[op.input_registers.left] *
        registers[op.input_registers.right];
      break;
    }
    case instruction_code::divide:
    {
      registers[op.result_register] =
        registers[op.input_registers.left] /
        registers[op.input_registers.right];
      break;
    }
    case instruction_code::negate:
    {
      registers[op.result_register] =
        -registers[op.input_registers.left];
      break;
    }
    case instruction_code::assign_constant:
    {
      registers[op.result_register] = op.constant;
      break;
    }
    case instruction_code::sqrt:
    {
      registers[op.result_register] =
        std::sqrt(registers[op.input_registers.left]);
      break;
    }
  }
}

}
