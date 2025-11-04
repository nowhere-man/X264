# ARM assembly sources
enable_language(ASM)

# C sources
aux_source_directory(. C_SRCS)

# Assembly sources
set(ASM_SRCS
    cpu-a.S
    bitstream-a.S
    dct-a.S
    deblock-a.S
    mc-a.S
    pixel-a.S
    predict-a.S
    quant-a.S
)

# Set assembly flags
set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -c")
if(CMAKE_POSITION_INDEPENDENT_CODE)
    set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -DPIC")
endif()

set(ARM_ASM_TARGETS)

if(HAVE_BITDEPTH8)
    add_library(asm_arm_8 OBJECT ${ASM_SRCS} ${C_SRCS})
    target_compile_definitions(asm_arm_8 PRIVATE HIGH_BIT_DEPTH=0 BIT_DEPTH=8)
    if(CMAKE_POSITION_INDEPENDENT_CODE)
        set_target_properties(asm_arm_8 PROPERTIES POSITION_INDEPENDENT_CODE ON)
        target_compile_definitions(asm_arm_8 PRIVATE PIC)
    endif()
    target_include_directories(asm_arm_8 PUBLIC
        ${PROJECT_BINARY_DIR}
        ${PROJECT_SOURCE_DIR}/
        ${PROJECT_SOURCE_DIR}/common/arm/
    )
    target_link_libraries(asm_arm_8 PRIVATE x264_options)
    list(APPEND ARM_ASM_TARGETS asm_arm_8)
endif()

if(HAVE_BITDEPTH10)
    add_library(asm_arm_10 OBJECT ${ASM_SRCS} ${C_SRCS})
    target_compile_definitions(asm_arm_10 PRIVATE HIGH_BIT_DEPTH=1 BIT_DEPTH=10)
    if(CMAKE_POSITION_INDEPENDENT_CODE)
        set_target_properties(asm_arm_10 PROPERTIES POSITION_INDEPENDENT_CODE ON)
        target_compile_definitions(asm_arm_10 PRIVATE PIC)
    endif()
    target_include_directories(asm_arm_10 PUBLIC
        ${PROJECT_BINARY_DIR}
        ${PROJECT_SOURCE_DIR}/
        ${PROJECT_SOURCE_DIR}/common/arm/
    )
    target_link_libraries(asm_arm_10 PRIVATE x264_options)
    list(APPEND ARM_ASM_TARGETS asm_arm_10)
endif()

set(COMMON_ASM_TARGETS "${ARM_ASM_TARGETS}" PARENT_SCOPE)
