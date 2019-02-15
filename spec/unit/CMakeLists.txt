cmake_minimum_required(VERSION 3.8)

file(GLOB TEST_SOURCE
          "../../src/*.cc"
          "../../src/base/*.cc"
          "../../src/doc/*.cc"
          "*.cc")
file(GLOB TEST_HEADERS
          "../../src/*.h"
          "../../src/base/*.h"
          "../../src/doc/*.h"
          "*.h")

find_package(Catch2 REQUIRED)
add_executable(nopodofo_tests Binding.cc)
target_link_libraries(nopodofo_tests Catch2::Catch2)

include(CTest)
include(Catch)
catch_discover_tests(nopodofo_tests)