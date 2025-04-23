aux_source_directory(. SRC_LIST)

if(NOT WIN32 OR NOT ENABLE_WIN32THREAD)
    list(REMOVE_ITEM SRC_LIST ./win32thread.c)
endif()

if(NOT ENABLE_OPENCL)
    list(REMOVE_ITEM SRC_LIST ./opencl.c)
else()
    set(SRC_H ${CMAKE_CURRENT_SOURCE_DIR}/opencl/oclobj.h)
    set(DST_H ${CMAKE_CURRENT_SOURCE_DIR}/oclobj.h)
    add_custom_command(
        OUTPUT ${DST_H}
        COMMAND ${CMAKE_COMMAND} -E copy ${SRC_H} ${DST_H}
        DEPENDS ${SRC_H}
        COMMENT "Generating oclobj.h"
    )
    add_custom_target(GenOclobj ALL DEPENDS ${DST_H})
endif()

if(ENABLE_ASM)
    if(X86)
        add_subdirectory(x86)
    elseif(ARM)
        add_subdirectory(arm)
    elseif(ARM64)
        add_subdirectory(aarch64)
    endif()
endif()

add_library(common OBJECT ${SRC_LIST} $<TARGET_OBJECTS:asm>)

target_include_directories(common PUBLIC
    ${PROJECT_SOURCE_DIR}/
    ${PROJECT_SOURCE_DIR}/common/
)