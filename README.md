Math Bytecode
=============
`math-bytecode` is a C++17 library that uses `ParseGen` to implement a compiler for a subset of the C language.
The language that `math-bytecode` supports is basically the body of a C function, allowing only variables of
type `double` and operations on them.
The body of the function is provided as a C++ `std::string` and is then compiled into bytecode stored in
an object of type `math_bytecode::compiled_function`.
The bytecode can then be executed by an object of type `math_bytecode::executable_function`
as if it were a regular C++ function call.
The executable function object is designed to be used in the interior of "hot loops" inside
physics simulation codes, and `math-bytecode` is an ideal way to accept arbitrary mathematical
functions from users for things like initial and boundary conditions.
`math-bytecode` uses `P3A` to be portably performant across HPC hardware like Intel CPUs,
NVIDIA GPUs, and AMD GPUs.
