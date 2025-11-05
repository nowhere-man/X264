# Architecture-Specific Compiler Flags Module
# This module sets up default compiler flags for specific architectures to match autotools behavior

# x86 (32-bit) specific flags
if(ARCH_X86)
    if(GCC OR CLANG)
        # Default to i686 and SSE/SSE2 for x86 (matching configure defaults)
        if(NOT CMAKE_C_FLAGS MATCHES "-march")
            x264_append_compile_options(-march=i686)
            message(STATUS "x86: Using default -march=i686")
        endif()
        if(NOT CMAKE_C_FLAGS MATCHES "-mfpmath")
            x264_append_compile_options(-mfpmath=sse -msse -msse2)
            message(STATUS "x86: Using default SSE/SSE2 optimizations")
        endif()
        # Windows x86 large address aware
        if(WIN32)
            x264_append_link_options(EXE -Wl,--large-address-aware)
            if(GCC)
                x264_append_link_options(EXE -Wl,--dynamicbase,--nxcompat,--tsaware)
            endif()
        endif()
    endif()
endif()

# ARM (32-bit) specific flags
if(ARCH_ARM)
    if(GCC OR CLANG)
        # Check if user already specified -mcpu, -march, or -mfpu
        if(NOT CMAKE_C_FLAGS MATCHES "-mcpu" AND NOT CMAKE_C_FLAGS MATCHES "-march" AND NOT CMAKE_C_FLAGS MATCHES "-mfpu")
            # Default to Cortex-A8 with NEON (matching configure defaults)
            x264_append_compile_options(-mcpu=cortex-a8 -mfpu=neon)
            message(STATUS "ARM: Using default -mcpu=cortex-a8 -mfpu=neon")
        endif()
    endif()
    
    if(SYS_MACOSX)
        # macOS ARM defaults to armv7
        if(NOT CMAKE_C_FLAGS MATCHES "-arch")
            x264_append_compile_options(-arch armv7)
            x264_append_link_options(BOTH -arch armv7)
            message(STATUS "macOS ARM: Using default -arch armv7")
        endif()
    endif()
endif()

# AArch64 (ARM64) specific flags
if(ARCH_AARCH64)
    if(SYS_MACOSX)
        # macOS ARM64
        if(NOT CMAKE_C_FLAGS MATCHES "-arch")
            x264_append_compile_options(-arch arm64)
            x264_append_link_options(BOTH -arch arm64)
            message(STATUS "macOS ARM64: Using -arch arm64")
        endif()
    endif()
endif()

# x86_64 specific flags
if(ARCH_X86_64)
    if(GCC OR CLANG)
        # Ensure 64-bit build
        if(NOT APPLE)
            if(NOT CMAKE_C_FLAGS MATCHES "-m64")
                x264_append_compile_options(-m64)
                x264_append_link_options(BOTH -m64)
            endif()
        endif()
        
        # Windows x86_64 security flags
        if(WIN32 AND GCC)
            x264_append_link_options(EXE -Wl,--dynamicbase,--nxcompat,--tsaware)
            x264_append_link_options(EXE -Wl,--image-base,0x140000000)
            x264_append_link_options(SHARED -Wl,--image-base,0x180000000)
        endif()
    endif()
    
    if(SYS_MACOSX)
        # macOS x86_64
        if(NOT CMAKE_C_FLAGS MATCHES "-arch x86_64")
            x264_append_compile_options(-arch x86_64)
            x264_append_link_options(BOTH -arch x86_64)
            message(STATUS "macOS x86_64: Using -arch x86_64")
        endif()
    endif()
endif()
