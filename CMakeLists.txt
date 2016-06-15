cmake_minimum_required(VERSION 2.8)

project(uvbench C CXX)

if(MSVC)
  set(BENCH_COMPILE_OPTIONS /W3 /WX /DUNICODE /D_UNICODE)

  foreach(flag
    CMAKE_C_FLAGS CMAKE_CXX_FLAGS
    CMAKE_C_FLAGS_DEBUG CMAKE_CXX_FLAGS_DEBUG
    CMAKE_C_FLAGS_RELEASE CMAKE_CXX_FLAGS_RELEASE
    CMAKE_C_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_MINSIZEREL 
    CMAKE_C_FLAGS_RELWITHDEBINFO CMAKE_CXX_FLAGS_RELWITHDEBINFO)
    string(REPLACE "/MD" "/MT" ${flag} "${${flag}}")
  endforeach()

else()
endif()

add_executable(uvbench main.cpp)
target_compile_options(uvbench PRIVATE ${BENCH_COMPILE_OPTIONS})
target_include_directories(uvbench PRIVATE libuv-1.9.1/include)
