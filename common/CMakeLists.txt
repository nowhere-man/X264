# Base source files (bit-depth independent)
set(SRCS_BASE
    osdep.c
    base.c
    cpu.c
    tables.c
)

# Bit-depth dependent source files
set(SRCS_X
    mc.c
    predict.c
    pixel.c
    macroblock.c
    frame.c
    dct.c
    cabac.c
    common.c
    rectangle.c
    set.c
    quant.c
    deblock.c
    vlc.c
    mvpred.c
    bitstream.c
)

set(SRCS_SHARED
    cJSON.c
    log.c
    dfx_capture.c
    timer.c
)

# OpenCL sources (8-bit only)
set(SRCS_8)
if(ENABLE_OPENCL)
    list(APPEND SRCS_8 opencl.c)

    # Generate oclobj.h from OpenCL kernel sources
    set(OPENCL_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/opencl/x264-cl.h)
    file(GLOB OPENCL_KERNELS ${CMAKE_CURRENT_SOURCE_DIR}/opencl/*.cl)
    set(OPENCL_SOURCES ${OPENCL_HEADER} ${OPENCL_KERNELS})

    set(OCLOBJ_H ${CMAKE_CURRENT_SOURCE_DIR}/oclobj.h)
    set(CLTOSTR_CMAKE ${PROJECT_SOURCE_DIR}/cmake/CLToStr.cmake)

    add_custom_command(
        OUTPUT ${OCLOBJ_H}
        COMMAND ${CMAKE_COMMAND}
            -DOUTPUT_FILE=${OCLOBJ_H}
            "-DINPUT_FILES=${OPENCL_SOURCES}"
            -P ${CLTOSTR_CMAKE}
        DEPENDS ${OPENCL_SOURCES} ${CLTOSTR_CMAKE}
        COMMENT "Generating oclobj.h from OpenCL kernel sources"
        VERBATIM
    )
    add_custom_target(GenOclobj ALL DEPENDS ${OCLOBJ_H})
endif()

# Threading support
if(HAVE_THREAD)
    list(APPEND SRCS_X threadpool.c)
endif()

if(HAVE_WIN32THREAD)
    list(APPEND SRCS_BASE win32thread.c)
endif()

# Export source lists to parent scope
set(COMMON_SRCS_BASE ${SRCS_BASE} PARENT_SCOPE)
set(COMMON_SRCS_X ${SRCS_X} PARENT_SCOPE)
set(COMMON_SRCS_8 ${SRCS_8} PARENT_SCOPE)

# Build base object library (bit-depth independent, no private_prefix)
add_library(common_base OBJECT ${SRCS_BASE})
target_include_directories(common_base PUBLIC ${PROJECT_BINARY_DIR} ${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/common)
if(BUILD_SHARED_LIBS)
    target_compile_definitions(common_base PRIVATE X264_API_EXPORTS)
endif()
target_link_libraries(common_base PRIVATE x264_options)
set(HAS_COMMON_BASE TRUE PARENT_SCOPE)

# Bit-depth independent sources compiled once
add_library(common_shared OBJECT ${SRCS_SHARED})
target_include_directories(common_shared PUBLIC
    ${PROJECT_BINARY_DIR}
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/common/
    ${PROJECT_SOURCE_DIR}/encoder/
)
if(HAVE_BITDEPTH8)
    target_compile_definitions(common_shared PRIVATE HIGH_BIT_DEPTH=0 BIT_DEPTH=8)
elseif(HAVE_BITDEPTH10)
    target_compile_definitions(common_shared PRIVATE HIGH_BIT_DEPTH=1 BIT_DEPTH=10)
else()
    target_compile_definitions(common_shared PRIVATE HIGH_BIT_DEPTH=0 BIT_DEPTH=8)
endif()
if(BUILD_SHARED_LIBS)
    target_compile_definitions(common_shared PRIVATE X264_API_EXPORTS)
endif()
target_link_libraries(common_shared PRIVATE x264_options)
set(HAS_COMMON_SHARED TRUE PARENT_SCOPE)
set(COMMON_SHARED_TARGET common_shared PARENT_SCOPE)

# Build bit-depth specific object libraries (WITH private_prefix in assembly)
if(HAVE_BITDEPTH8)
    add_library(common_8 OBJECT ${SRCS_X} ${SRCS_8})
    target_compile_definitions(common_8 PRIVATE HIGH_BIT_DEPTH=0 BIT_DEPTH=8)
    target_include_directories(common_8 PUBLIC ${PROJECT_BINARY_DIR} ${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/common)
    if(BUILD_SHARED_LIBS)
        target_compile_definitions(common_8 PRIVATE X264_API_EXPORTS)
    endif()
    target_link_libraries(common_8 PRIVATE x264_options)
    if(ENABLE_OPENCL)
        add_dependencies(common_8 GenOclobj)
    endif()
    set(HAS_COMMON_8 TRUE PARENT_SCOPE)
endif()

if(HAVE_BITDEPTH10)
    add_library(common_10 OBJECT ${SRCS_X})
    target_compile_definitions(common_10 PRIVATE HIGH_BIT_DEPTH=1 BIT_DEPTH=10)
    target_include_directories(common_10 PUBLIC ${PROJECT_BINARY_DIR} ${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/common)
    if(BUILD_SHARED_LIBS)
        target_compile_definitions(common_10 PRIVATE X264_API_EXPORTS)
    endif()
    target_link_libraries(common_10 PRIVATE x264_options)
    set(HAS_COMMON_10 TRUE PARENT_SCOPE)
endif()

# Note: The 'common' target is created later by combining these

set(_common_asm_targets "")
if(ENABLE_ASM)
    if(X86)
        add_subdirectory(x86)
    elseif(ARM)
        add_subdirectory(arm)
    elseif(ARM64)
        add_subdirectory(aarch64)
    endif()

    if(DEFINED COMMON_ASM_TARGETS)
        set(_common_asm_targets "${COMMON_ASM_TARGETS}")
    endif()
endif()

if(_common_asm_targets)
    set(HAS_ASM_TARGET TRUE PARENT_SCOPE)
    set(ASM_TARGET_NAMES "${_common_asm_targets}" PARENT_SCOPE)
else()
    set(HAS_ASM_TARGET FALSE PARENT_SCOPE)
    set(ASM_TARGET_NAMES "" PARENT_SCOPE)
endif()
