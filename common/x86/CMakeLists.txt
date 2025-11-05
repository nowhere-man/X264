# C sources
aux_source_directory(. C_SRCS)

# Assembly sources (bit-depth independent)
set(ASM_SRCS cpu-a.asm)

# Assembly sources for 8-bit
set(ASM_SRCS_8 const-a.asm cabac-a.asm dct-a.asm deblock-a.asm mc-a.asm
    mc-a2.asm pixel-a.asm predict-a.asm quant-a.asm bitstream-a.asm sad-a.asm
)

# Assembly sources for 10-bit (same as 8-bit but use sad16-a instead of sad-a)
set(ASM_SRCS_10 const-a.asm cabac-a.asm dct-a.asm deblock-a.asm mc-a.asm
    mc-a2.asm pixel-a.asm predict-a.asm quant-a.asm bitstream-a.asm sad16-a.asm
)

# Platform-specific assembly
if(ARCH_X86_64)
    list(APPEND ASM_SRCS_8 dct-64.asm trellis-64.asm)
    list(APPEND ASM_SRCS_10 dct-64.asm trellis-64.asm)
else()
    list(APPEND ASM_SRCS_8 dct-32.asm pixel-32.asm)
    list(APPEND ASM_SRCS_10 dct-32.asm pixel-32.asm)
endif()

# set asm_nasm flags
set(CMAKE_ASM_NASM_FLAGS_INIT "")
set(CMAKE_ASM_NASM_COMPILE_OBJECT "<CMAKE_ASM_NASM_COMPILER> <INCLUDES> <FLAGS> -o <OBJECT> <SOURCE>")

# Set format based on platform
if(CMAKE_SIZEOF_VOID_P MATCHES 8)
    if(APPLE)
        set(CMAKE_ASM_NASM_OBJECT_FORMAT macho64)
        set(PREFIX_FLAG "-DPREFIX")
    elseif(UNIX)
        set(CMAKE_ASM_NASM_OBJECT_FORMAT elf64)
        set(PREFIX_FLAG "")
    elseif(WIN32)
        set(CMAKE_ASM_NASM_OBJECT_FORMAT win64)
        set(PREFIX_FLAG "")
    endif()
    set(ARCH_FLAG "-DARCH_X86_64=1")
else()
    if(APPLE)
        set(CMAKE_ASM_NASM_OBJECT_FORMAT macho32)
        set(PREFIX_FLAG "-DPREFIX")
    elseif(UNIX)
        set(CMAKE_ASM_NASM_OBJECT_FORMAT elf32)
        set(PREFIX_FLAG "")
    elseif(WIN32)
        set(CMAKE_ASM_NASM_OBJECT_FORMAT win32)
        set(PREFIX_FLAG "-DPREFIX")
    endif()
    set(ARCH_FLAG "-DARCH_X86_64=0")
endif()

# Common ASM flags
set(ASM_COMMON_FLAGS "-f ${CMAKE_ASM_NASM_OBJECT_FORMAT} ${PREFIX_FLAG} ${ARCH_FLAG} -DSTACK_ALIGNMENT=${X264_STACK_ALIGNMENT}")
if(CMAKE_POSITION_INDEPENDENT_CODE)
    set(ASM_COMMON_FLAGS "${ASM_COMMON_FLAGS} -DPIC")
endif()

# Set include directories for NASM and construct per-target object lists
set(X86_ASM_TARGETS)

if(ASM_SRCS)
    add_library(asm_x86_base OBJECT ${ASM_SRCS})
    target_compile_options(asm_x86_base PRIVATE
        $<$<COMPILE_LANGUAGE:ASM_NASM>:SHELL:${ASM_COMMON_FLAGS}>
    )
    target_include_directories(asm_x86_base PUBLIC
        ${PROJECT_BINARY_DIR}
        ${PROJECT_SOURCE_DIR}/
        ${PROJECT_SOURCE_DIR}/common/
        ${PROJECT_SOURCE_DIR}/common/x86/
    )
    target_link_libraries(asm_x86_base PRIVATE x264_options)
    list(APPEND X86_ASM_TARGETS asm_x86_base)
endif()

if(HAVE_BITDEPTH8)
    add_library(asm_x86_8 OBJECT ${ASM_SRCS_8} ${C_SRCS})
    target_compile_options(asm_x86_8 PRIVATE
        $<$<COMPILE_LANGUAGE:ASM_NASM>:SHELL:${ASM_COMMON_FLAGS} -DBIT_DEPTH=8 -Dprivate_prefix=x264_8>
    )
    target_compile_definitions(asm_x86_8 PRIVATE HIGH_BIT_DEPTH=0 BIT_DEPTH=8)
    target_include_directories(asm_x86_8 PUBLIC
        ${PROJECT_BINARY_DIR}
        ${PROJECT_SOURCE_DIR}/
        ${PROJECT_SOURCE_DIR}/common/
        ${PROJECT_SOURCE_DIR}/common/x86/
    )
    target_link_libraries(asm_x86_8 PRIVATE x264_options)
    list(APPEND X86_ASM_TARGETS asm_x86_8)
endif()

if(HAVE_BITDEPTH10)
    add_library(asm_x86_10 OBJECT ${ASM_SRCS_10} ${C_SRCS})
    target_compile_options(asm_x86_10 PRIVATE
        $<$<COMPILE_LANGUAGE:ASM_NASM>:SHELL:${ASM_COMMON_FLAGS} -DBIT_DEPTH=10 -Dprivate_prefix=x264_10>
    )
    target_compile_definitions(asm_x86_10 PRIVATE HIGH_BIT_DEPTH=1 BIT_DEPTH=10)
    target_include_directories(asm_x86_10 PUBLIC
        ${PROJECT_BINARY_DIR}
        ${PROJECT_SOURCE_DIR}/
        ${PROJECT_SOURCE_DIR}/common/
        ${PROJECT_SOURCE_DIR}/common/x86/
    )
    target_link_libraries(asm_x86_10 PRIVATE x264_options)
    list(APPEND X86_ASM_TARGETS asm_x86_10)
endif()

set(COMMON_ASM_TARGETS "${X86_ASM_TARGETS}" PARENT_SCOPE)
