cmake_minimum_required(VERSION 2.8)
project(uvbench C CXX)

if(MSVC)
    foreach(flag
        CMAKE_C_FLAGS CMAKE_CXX_FLAGS
        CMAKE_C_FLAGS_DEBUG CMAKE_CXX_FLAGS_DEBUG
        CMAKE_C_FLAGS_RELEASE CMAKE_CXX_FLAGS_RELEASE
        CMAKE_C_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_MINSIZEREL
        CMAKE_C_FLAGS_RELWITHDEBINFO CMAKE_CXX_FLAGS_RELWITHDEBINFO)
        string(REPLACE "/MD" "/MT" ${flag} "${${flag}}")
    endforeach()
endif()

include(external/http-parser.cmake)
include(external/libuv.cmake)

if(MSVC)
    set(UVBENCH_LIBS Ws2_32.lib Psapi.lib UserEnv.lib Iphlpapi.lib)
else()
    set(UVBENCH_OPTS -std=c++11)
endif()

add_executable(uvbench
    src/Main.cpp
    src/UVWrap.cpp
    src/Server.cpp
    src/Connection.cpp
)

add_dependencies(uvbench libuv)
target_compile_options(uvbench PRIVATE ${UVBENCH_OPTS})
target_include_directories(uvbench PRIVATE ${LIBUV_INC})
target_link_libraries(uvbench http-parser ${LIBUV_LIB} ${UVBENCH_LIBS})
