cmake_minimum_required(VERSION 3.12.0...3.18.0)

project(RTC VERSION 0.1.0 LANGUAGES CXX)

find_package(ParseGen REQUIRED)

add_library(runtime_compiler
  runtime_compiler.cpp
  )

target_link_libraries(runtime_compiler PRIVATE parsegen)
