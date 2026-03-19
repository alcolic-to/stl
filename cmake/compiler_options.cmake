if (MSVC)
    add_compile_definitions($<$<CONFIG:Debug>:DEBUG>)
else()
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_definitions(-DDEBUG)
    endif()
endif()

# Next section is compiler options.

# Optional, using libc++ with clang, for better debugging with lldb-dap.
OPTION(STL_USE_LIBCPP "If set, sets compiler stdlib value to libc++." ON)
if (STL_USE_LIBCPP AND CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
else()
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wpedantic")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wtype-limits")
    # SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wshadow") # warn the user if a variable declaration shadows one from a parent context
    # SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wnon-virtual-dtor") # warn the user if a class with virtual functions has a non-virtual destructor. This helps catch hard to track down memory errors
    # SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wold-style-cast") # warn for c-style casts
    # SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wcast-align") # warn for potential performance problem casts
    # SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunused") # warn on anything being unused
    # SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Woverloaded-virtual") # warn if you overload (not override) a virtual function
    # SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wconversion") # warn on type conversions that may lose data
    # SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wsign-conversion") # warn on sign conversions
    # SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wnull-dereference") # warn if a null dereference is detected
    # SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wdouble-promotion") # warn if float is implicit promoted to double
    # SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wformat=2") # warn on security issues around functions that format output (ie printf)
    # SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wimplicit-fallthrough") # warn on statements that fallthrough without an explicit annotation

    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-interference-size")
    endif()
endif()
