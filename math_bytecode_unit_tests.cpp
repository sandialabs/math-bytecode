#include <gtest/gtest.h>
#include <Kokkos_Core.hpp>

#include "math_bytecode.hpp"

TEST(compiled_function, copy_to_device)
{
  math_bytecode::host_function hf = math_bytecode::compile(
      "void myfunction(const double in[2], double out[2]) {\n"
      "  out[0] = in[0];\n"
      "  out[1] = in[1];\n"
      "}\n");
  math_bytecode::device_function df(hf);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  Kokkos::ScopeGuard kokkos_library_state(argc, argv);
  return RUN_ALL_TESTS();
}
