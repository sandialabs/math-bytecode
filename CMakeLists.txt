cmake_minimum_required(VERSION 3.12.0...3.18.0)

project(rtc VERSION 1.0.0 LANGUAGES CXX)

if(WIN32 AND NOT CYGWIN)
  set(INSTALL_CMAKE_DIR CMake)
  set(INSTALL_BIN_DIR ".")
  set(INSTALL_LIB_DIR ".")
else()
  set(INSTALL_CMAKE_DIR lib/cmake/${PROJECT_NAME})
  set(INSTALL_BIN_DIR "bin")
  set(INSTALL_LIB_DIR "lib")
endif()

find_package(parsegen REQUIRED)
find_package(p3a REQUIRED)
find_package(MPI REQUIRED)

if (p3a_ENABLE_CUDA)
  enable_language(CUDA)
  set_source_files_properties(runtime_compiler.cpp PROPERTIES LANGUAGE CUDA)
endif()

add_library(runtime-compiler runtime_compiler.cpp)

target_compile_features(runtime-compiler PUBLIC cxx_std_17)

set_target_properties(runtime-compiler PROPERTIES
  PUBLIC_HEADER runtime_compiler.hpp
  OUTPUT_NAME runtime_compiler)
target_include_directories(runtime-compiler
  INTERFACE $<INSTALL_INTERFACE:include>)

target_link_libraries(runtime-compiler PRIVATE parsegen)
target_link_libraries(runtime-compiler PUBLIC p3a)

install(TARGETS runtime-compiler EXPORT rtc-targets)

configure_file(rtc-config.cmake.in
  "${PROJECT_BINARY_DIR}/rtc-config.cmake" @ONLY)
configure_file(rtc-config-version.cmake.in
  "${PROJECT_BINARY_DIR}/rtc-config-version.cmake" @ONLY)
install(FILES
  "${PROJECT_BINARY_DIR}/rtc-config.cmake"
  "${PROJECT_BINARY_DIR}/rtc-config-version.cmake"
  DESTINATION "${INSTALL_CMAKE_DIR}")
install(EXPORT rtc-targets
  DESTINATION "${INSTALL_CMAKE_DIR}")
