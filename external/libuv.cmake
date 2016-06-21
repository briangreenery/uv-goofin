set(LIBUV_DIR libuv-1.9.1)

if(CMAKE_BUILD_TYPE STREQUAL Release)
    set(LIBUV_BUILDTYPE Release)
else()
    set(LIBUV_BUILDTYPE Debug)
endif()

set(LIBUV_INC ${CMAKE_SOURCE_DIR}/external/${LIBUV_DIR}/include)
set(LIBUV_LIB ${CMAKE_SOURCE_DIR}/external/${LIBUV_DIR}/out/${LIBUV_BUILDTYPE}/libuv.a)

add_custom_command(
    OUTPUT ${LIBUV_LIB}
    WORKING_DIRECTORY external/${LIBUV_DIR}
    COMMAND ./gyp_uv.py -f make
    COMMAND "BUILDTYPE=${LIBUV_BUILDTYPE}" make -C out
)

add_custom_target(libuv DEPENDS ${LIBUV_LIB})
