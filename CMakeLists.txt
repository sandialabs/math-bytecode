cmake_minimum_required(VERSION 3.12.0...3.18.0)

project(RTC VERSION 0.1.0 LANGUAGES CXX)

option(RTC_ENABLE_CUDA "Build GPU version with CUDA" OFF)

if(WIN32 AND NOT CYGWIN)
  set(INSTALL_CMAKE_DIR CMake)
  set(INSTALL_BIN_DIR ".")
  set(INSTALL_LIB_DIR ".")
else()
  set(INSTALL_CMAKE_DIR lib/cmake/${PROJECT_NAME})
  set(INSTALL_BIN_DIR "bin")
  set(INSTALL_LIB_DIR "lib")
endif()

if (RTC_ENABLE_CUDA)
  enable_language(CUDA)
endif()

find_package(ParseGen REQUIRED)
find_package(P3A REQUIRED)

if (RTC_ENABLE_CUDA)
  set_source_files_properties(runtime_compiler.cpp PROPERTIES LANGUAGE CUDA)
endif()

add_library(runtime_compiler runtime_compiler.cpp)

target_compile_features(runtime_compiler PUBLIC cxx_std_17)

set_target_properties(runtime_compiler PROPERTIES
  PUBLIC_HEADER runtime_compiler.hpp)
target_include_directories(runtime_compiler
  INTERFACE $<INSTALL_INTERFACE:include>)

target_link_libraries(runtime_compiler PRIVATE parsegen)
target_link_libraries(runtime_compiler PUBLIC p3a)

install(TARGETS runtime_compiler
  EXPORT RTCTargets
  LIBRARY DESTINATION "${INSTALL_LIB_DIR}"
  ARCHIVE DESTINATION "${INSTALL_LIB_DIR}"
  RUNTIME DESTINATION "${INSTALL_LIB_DIR}"
  PUBLIC_HEADER DESTINATION "include"
  )

configure_file(RTCConfig.cmake.in
  "${PROJECT_BINARY_DIR}/RTCConfig.cmake" @ONLY)
configure_file(RTCConfigVersion.cmake.in
  "${PROJECT_BINARY_DIR}/RTCConfigVersion.cmake" @ONLY)
install(FILES
  "${PROJECT_BINARY_DIR}/RTCConfig.cmake"
  "${PROJECT_BINARY_DIR}/RTCConfigVersion.cmake"
  DESTINATION "${INSTALL_CMAKE_DIR}")
install(EXPORT RTCTargets
  DESTINATION "${INSTALL_CMAKE_DIR}")
