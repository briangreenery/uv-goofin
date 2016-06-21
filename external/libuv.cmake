set(LIBUV_DIR libuv-1.9.1)
set(LIBUV_INC ${CMAKE_SOURCE_DIR}/external/${LIBUV_DIR}/include)

if(MSVC)
    set(LIBUV_LIB ${CMAKE_SOURCE_DIR}/external/${LIBUV_DIR}/Release/lib/libuv.lib)

    add_custom_command(
        OUTPUT ${LIBUV_LIB}
        WORKING_DIRECTORY external/${LIBUV_DIR}
        COMMAND vcbuild Release x64
    )
else()
    set(LIBUV_LIB ${CMAKE_SOURCE_DIR}/external/${LIBUV_DIR}/out/Release/libuv.a)

    add_custom_command(
        OUTPUT ${LIBUV_LIB}
        WORKING_DIRECTORY external/${LIBUV_DIR}
        COMMAND ./gyp_uv.py -f make
        COMMAND "BUILDTYPE=Release" make -C out
    )
endif()

add_custom_target(libuv DEPENDS ${LIBUV_LIB})
