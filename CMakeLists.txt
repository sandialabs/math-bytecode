cmake_minimum_required(VERSION 3.12.0...3.18.0)

project(math-bytecode VERSION 1.0.0 LANGUAGES CXX)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

find_package(parsegen REQUIRED)
find_package(p3a REQUIRED)

if (BUILD_TESTING)
  enable_testing()
  find_package(GTest REQUIRED)
endif()

enable_language(${p3a_LANGUAGE})

set_source_files_properties(math_bytecode.cpp PROPERTIES LANGUAGE ${p3a_LANGUAGE})
add_library(math-bytecode math_bytecode.cpp)
target_compile_features(math-bytecode PUBLIC cxx_std_17)
set_target_properties(math-bytecode PROPERTIES ${p3a_LANGUAGE}_ARCHITECTURES "${p3a_ARCHITECTURES}")
set_target_properties(math-bytecode PROPERTIES
  PUBLIC_HEADER math_bytecode.hpp
  OUTPUT_NAME math_bytecode)
target_include_directories(math-bytecode
  PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")
target_link_libraries(math-bytecode PRIVATE parsegen::parsegen)
target_link_libraries(math-bytecode PUBLIC p3a::p3a)

install(TARGETS math-bytecode EXPORT math-bytecode-targets)

configure_package_config_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/config.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/math-bytecode-config.cmake"
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/math-bytecode
)
write_basic_package_version_file(
  "${PROJECT_BINARY_DIR}/math-bytecode-config-version.cmake"
  COMPATIBILITY AnyNewerVersion)

install(FILES
  "${PROJECT_BINARY_DIR}/math-bytecode-config.cmake"
  "${PROJECT_BINARY_DIR}/math-bytecode-config-version.cmake"
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/math-bytecode)

install(
  EXPORT math-bytecode-targets
  NAMESPACE math-bytecode::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/math-bytecode)

if (BUILD_TESTING)
  set(unit_test_sources
    math_bytecode_unit_tests.cpp
    )
  set_source_files_properties(
    ${unit_test_sources} PROPERTIES LANGUAGE ${p3a_LANGUAGE})
  add_executable(math-bytecode-unit-tests ${unit_test_sources})
  set_target_properties(math-bytecode-unit-tests PROPERTIES ${p3a_LANGUAGE}_ARCHITECTURES "${p3a_ARCHITECTURES}")
  target_link_libraries(math-bytecode-unit-tests PRIVATE math-bytecode)
  target_link_libraries(math-bytecode-unit-tests PRIVATE GTest::gtest)
  add_test(NAME unit-tests COMMAND math-bytecode-unit-tests)
endif()

