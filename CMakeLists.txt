cmake_minimum_required(VERSION 3.12.0...3.18.0)

project(math-bytecode VERSION 1.0.0 LANGUAGES CXX)

if(WIN32 AND NOT CYGWIN)
  set(INSTALL_CMAKE_DIR CMake)
else()
  set(INSTALL_CMAKE_DIR lib/cmake/${PROJECT_NAME})
endif()

find_package(parsegen REQUIRED)
find_package(p3a REQUIRED)

enable_language(${p3a_LANGUAGE})

set_source_files_properties(math_bytecode.cpp PROPERTIES LANGUAGE ${p3a_LANGUAGE})
add_library(math-bytecode math_bytecode.cpp)
target_compile_features(math-bytecode PUBLIC cxx_std_17)
set_target_properties(math-bytecode PROPERTIES ${p3a_LANGUAGE}_ARCHITECTURES "${p3a_ARCHITECTURES}")
set_target_properties(math-bytecode PROPERTIES
  PUBLIC_HEADER math_bytecode.hpp
  OUTPUT_NAME math_bytecode)
target_include_directories(math-bytecode
  INTERFACE $<INSTALL_INTERFACE:include>)
target_link_libraries(math-bytecode PRIVATE parsegen::parsegen)
target_link_libraries(math-bytecode PUBLIC p3a::p3a)

install(TARGETS math-bytecode EXPORT math-bytecode-targets)

configure_file(math-bytecode-config.cmake.in
  "${PROJECT_BINARY_DIR}/math-bytecode-config.cmake" @ONLY)
configure_file(math-bytecode-config-version.cmake.in
  "${PROJECT_BINARY_DIR}/math-bytecode-config-version.cmake" @ONLY)
install(FILES
  "${PROJECT_BINARY_DIR}/math-bytecode-config.cmake"
  "${PROJECT_BINARY_DIR}/math-bytecode-config-version.cmake"
  DESTINATION "${INSTALL_CMAKE_DIR}")
install(EXPORT math-bytecode-targets
  DESTINATION "${INSTALL_CMAKE_DIR}")
