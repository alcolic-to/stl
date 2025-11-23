OPTION(STL_BUILD_TEST "If set, builds all tests." OFF)
if (NOT STL_BUILD_TEST)
    return()
endif()

include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/52eb8108c5bdec04579160ae17225d66034bd723.zip) # Release 1.17.0

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/$<CONFIG>/test)

message("Disabling clang-tidy for google tests.")
set_target_properties(gmock PROPERTIES CXX_CLANG_TIDY "")
set_target_properties(gmock_main PROPERTIES CXX_CLANG_TIDY "")
set_target_properties(gtest PROPERTIES CXX_CLANG_TIDY "")
set_target_properties(gtest_main PROPERTIES CXX_CLANG_TIDY "")

function(add_stl_test TEST_FILE)
    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
    add_executable(${TEST_NAME} ${TEST_FILE})
    target_link_libraries(${TEST_NAME} ${STL_LIB} gtest_main)
    target_include_directories(${TEST_NAME} PUBLIC ${CMAKE_SOURCE_DIR})
    target_compile_definitions(${TEST_NAME} PRIVATE TEST_ROOT="${CMAKE_SOURCE_DIR}/test")
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endfunction()

add_stl_test("test_intrusive_list.cpp")
add_stl_test("test_small_string.cpp")
add_stl_test("test_array_map.cpp")
