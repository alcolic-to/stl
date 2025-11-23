# Usage:
# Create initial cmake build folder: cmake [-G Ninja] -B build -DCMAKE_BUILD_TYPE=[Debug/Release] -DCMAKE_CXX_COMPILER=clang++-20 -DCMAKE_C_COMPILER=clang-20
# Create build with desired config: cmake --build build
#
# If you want to play around with stl, you can just enable STL_BUILD_MAIN which builds main.cpp file.
#
cmake_minimum_required(VERSION 3.10)
project(stl
        DESCRIPTION "STL project is an extension to c++ STL library."
        VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # -> compile_commands.json

# Separate bin directory from build directory.
#
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/$<CONFIG>)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/$<CONFIG>)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/$<CONFIG>)

include(cmake/project_config.cmake)
include(cmake/sanitizers.cmake)
include(cmake/compiler_options.cmake)
include(cmake/profilers.cmake)

set(HPP_FILES types.hpp util.hpp intrusive_list.hpp small_string.hpp ptr_tag.hpp array_map.hpp art.hpp ast.hpp)

OPTION(STL_BUILD_MAIN "If set, builds main.cpp for manual scheduler testing." OFF)
if (STL_BUILD_MAIN)
    add_executable(stl_main main.cpp)
    target_include_directories(stl_main PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
endif()

include(CTest)
add_subdirectory(test)
add_subdirectory(benchmark)
