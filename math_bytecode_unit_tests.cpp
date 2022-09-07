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

TEST(execute, on_host)
{
  auto host_function = math_bytecode::compile(
      "void density(const double x[3], double& rho) {\n"
      "  rho = 1.0 + x[0];\n"
      "}\n");
  auto exe_function = host_function.executable();
  double registers[10];
  double const x[3] = {0, 0, 0};
  double rho;
  exe_function(registers, x, rho);
  EXPECT_EQ(rho, 1.0);
}

TEST(execute, vector3_double)
{
  auto host_function = math_bytecode::compile(
      "void density(const double x[3], double& rho) {\n"
      "  rho = 1.0 + x[0];\n"
      "}\n");
  auto exe_function = host_function.executable();
  double registers[10];
  p3a::vector3<double> const x(0, 0, 0);
  double rho;
  exe_function(registers, x, rho);
  EXPECT_EQ(rho, 1.0);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  Kokkos::ScopeGuard kokkos_library_state(argc, argv);
  return RUN_ALL_TESTS();
}
