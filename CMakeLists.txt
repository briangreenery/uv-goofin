cmake_minimum_required(VERSION 2.8)

project(uvbench C CXX)

set(LIBUV libuv-1.9.1)

if(MSVC)
  set(BENCH_COMPILE_OPTIONS /W3 /WX /DUNICODE /D_UNICODE)

  set(BENCH_LIBS
    ${LIBUV}/Release/lib/libuv
    Psapi.lib
    Ws2_32.lib
    Userenv.lib
    Iphlpapi.lib
  )

  foreach(flag
    CMAKE_C_FLAGS CMAKE_CXX_FLAGS
    CMAKE_C_FLAGS_DEBUG CMAKE_CXX_FLAGS_DEBUG
    CMAKE_C_FLAGS_RELEASE CMAKE_CXX_FLAGS_RELEASE
    CMAKE_C_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_MINSIZEREL 
    CMAKE_C_FLAGS_RELWITHDEBINFO CMAKE_CXX_FLAGS_RELWITHDEBINFO)
    string(REPLACE "/MD" "/MT" ${flag} "${${flag}}")
  endforeach()

else()
  set(BENCH_COMPILE_OPTIONS -Wall -Werror -O3)

  set(BENCH_LIBS
    ${CMAKE_SOURCE_DIR}/${LIBUV}/out/Release/libuv.a
    pthread
    rt
  )
endif()

add_executable(uvbench
  src/main.cpp
)

target_compile_options(uvbench PRIVATE ${BENCH_COMPILE_OPTIONS})
target_include_directories(uvbench PRIVATE ${LIBUV}/include)
target_link_libraries(uvbench ${BENCH_LIBS})
