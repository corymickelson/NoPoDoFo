cmake_minimum_required(VERSION 3.8)

find_package(Catch2 REQUIRED)
add_executable(nopodofo_tests Binding.cc)
target_link_libraries(nopodofo_tests 
	PRIVATE
	Catch2::Catch2
	nopodofo)

include(CTest)
include(Catch)
catch_discover_tests(nopodofo_tests)