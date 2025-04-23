enable_language(ASM_NASM)

# add c src lists
aux_source_directory(. C_SRCS)

# add asm src lists
set(ASM_SRCS cpu-a.asm)
set(ASM_SRCS_8 const-a.asm cabac-a.asm dct-a.asm deblock-a.asm mc-a.asm
    mc-a2.asm pixel-a.asm predict-a.asm quant-a.asm bitstream-a.asm
)
if(HIGH_BIT_DEPTH)
    list(APPEND ASM_SRCS_8 sad16-a.asm)
else()
    list(APPEND ASM_SRCS_8 sad-a.asm)
endif()
if(X86_64)
    list(APPEND ASM_SRCS_8 dct-64.asm trellis-64.asm )
else()
    list(APPEND ASM_SRCS_8 dct-32.asm pixel-32.asm)
endif()

# set asm_nasm flags
list(APPEND ASM_FLAGS -I${PROJECT_SOURCE_DIR}/common/ -I${PROJECT_SOURCE_DIR}/common/x86/ -Ox)
list(APPEND ASM_FLAGS -DSTACK_ALIGNMENT=${X264_STACK_ALIGNMENT})
if(CMAKE_POSITION_INDEPENDENT_CODE)
    list(APPEND ASM_FLAGS -DPIC)
endif()
if(CMAKE_SIZEOF_VOID_P MATCHES 8)
    list(APPEND ASM_FLAGS -DARCH_X86_64=1)
    if(APPLE)
        list(PREPEND ASM_FLAGS -f macho64)
    elseif(LINUX)
      list(PREPEND ASM_FLAGS -f elf64)
    elseif(WIN32)
        list(PREPEND ASM_FLAGS -f win64)
    endif()
else()
    list(PREPEND ASM_FLAGS -DARCH_X86_64=0)
    if(LINUX)
        list(PREPEND ASM_FLAGS -f elf32)
    elseif(WIN32)
        list(PREPEND ASM_FLAGS -f win32)
    endif()
endif()

string(REPLACE ";" " " ASM_FLAGS "${ASM_FLAGS}")
set_source_files_properties(${ASM_SRCS} PROPERTIES COMPILE_FLAGS ${ASM_FLAGS})
set_source_files_properties(${ASM_SRCS_8} PROPERTIES COMPILE_FLAGS "${ASM_FLAGS} -DBIT_DEPTH=8 -Dprivate_prefix=x264_8")

add_library(asm OBJECT ${ASM_SRCS} ${ASM_SRCS_8} ${C_SRCS})


target_include_directories(asm PUBLIC
    ${PROJECT_SOURCE_DIR}/
    ${PROJECT_SOURCE_DIR}/common/x86/
)