cmake_minimum_required(VERSION 3.20)

project(X264)

set(X264_BUILD 164)

# set build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()
message("CMAKE_BUILD_TYPE: " ${CMAKE_BUILD_TYPE})

# export compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# include modules
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(CheckCCompilerFlag)
include(CheckCSourceCompiles)
include(CheckCSourceRuns)
include(CheckIncludeFile)
include(CheckFunctionExists)
include(CheckSymbolExists)
include(X264Utils)

# build options
set(CMAKE_SKIP_RPATH TRUE)
option(BUILD_SHARED_LIBS  "Build shared library"                              ON )
option(ENABLE_CLI         "Enable x264 cli executable"                        ON )
option(ENABLE_ASM         "Enable assembly optimizations"                     ON )
option(ENABLE_OPENCL      "Enable OpenCL features"                            ON )
option(ENABLE_GPL         "Enable GPL-only features"                          ON )
option(ENABLE_THREAD      "Enable multithreaded encoding"                     ON )
option(ENABLE_WIN32THREAD "Enable win32threads(windows only)"                 ON )
option(ENABLE_INTERLACED  "Enable interlaced encoding"                        ON )
option(ENABLE_LTO         "Enable link-time optimizations"                    OFF)
option(ENABLE_ASAN        "Enable address sanitizer"                          OFF)
option(ENABLE_PIC         "Build position-independent code"                   OFF)

# external libs
set(ENABLE_AVS     "auto" CACHE STRING "Avisynth support (auto/on/off)")
set_property(CACHE ENABLE_AVS PROPERTY STRINGS auto on off)
set(ENABLE_SWSCALE "auto" CACHE STRING "libswscale support (auto/on/off)")
set_property(CACHE ENABLE_SWSCALE PROPERTY STRINGS auto on off)
set(ENABLE_LAVF    "auto" CACHE STRING "libavformat support (auto/on/off)")
set_property(CACHE ENABLE_LAVF PROPERTY STRINGS auto on off)
set(ENABLE_FFMS    "auto" CACHE STRING "ffmpegsource support (auto/on/off)")
set_property(CACHE ENABLE_FFMS PROPERTY STRINGS auto on off)
set(ENABLE_GPAC    "auto" CACHE STRING "GPAC support (auto/on/off)")
set_property(CACHE ENABLE_GPAC PROPERTY STRINGS auto on off)
set(ENABLE_LSMASH  "auto" CACHE STRING "L-SMASH support (auto/on/off)")
set_property(CACHE ENABLE_LSMASH PROPERTY STRINGS auto on off)

# determine whether PIC is required
set(X264_ENABLE_PIC OFF)
if(BUILD_SHARED_LIBS)
    set(X264_ENABLE_PIC ON)
elseif(ENABLE_PIC)
    set(X264_ENABLE_PIC ON)
endif()
if(X264_ENABLE_PIC)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
else()
    set(CMAKE_POSITION_INDEPENDENT_CODE OFF)
endif()

# Bit depth (8, 10, all) [8 by default]
if(NOT DEFINED X264_BIT_DEPTH)
    set(X264_BIT_DEPTH "8" CACHE STRING "Output bit depth (8, 10, or all)")
    set_property(CACHE X264_BIT_DEPTH PROPERTY STRINGS "8" "10" "all")
endif()
if(NOT X264_BIT_DEPTH MATCHES "^(8|10|all)$")
    message(FATAL_ERROR "X264_BIT_DEPTH must be 8, 10, or all (got: ${X264_BIT_DEPTH})")
endif()

# Chroma format (400, 420, 422, 444, all) [420 by default]
if(NOT DEFINED X264_CHROMA_FORMAT)
    set(X264_CHROMA_FORMAT "420" CACHE STRING "Output chroma format (400, 420, 422, 444, or all)")
    set_property(CACHE X264_CHROMA_FORMAT PROPERTY STRINGS "400" "420" "422" "444" "all")
endif()
if(NOT X264_CHROMA_FORMAT MATCHES "^(400|420|422|444|all)$")
    message(FATAL_ERROR "X264_CHROMA_FORMAT must be 400, 420, 422, 444, or all (got: ${X264_CHROMA_FORMAT})")
endif()

# Chroma format
if(X264_CHROMA_FORMAT STREQUAL "all")
    set(X264_CHROMA_FLAG 0)
else()
    set(X264_CHROMA_FLAG "X264_CSP_I${X264_CHROMA_FORMAT}")
endif()

# build directories
set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/install)
set(CMAKE_INSTALL_BINDIR ${CMAKE_INSTALL_PREFIX}/bin)
set(CMAKE_INSTALL_LIBDIR ${CMAKE_INSTALL_PREFIX}/lib)
set(CMAKE_INSTALL_INCLUDEDIR ${CMAKE_INSTALL_PREFIX}/include)
message("CMAKE_INSTALL_PREFIX          : " ${CMAKE_INSTALL_PREFIX})
message("CMAKE_INSTALL_BINDIR          : " ${CMAKE_INSTALL_BINDIR})
message("CMAKE_INSTALL_LIBDIR          : " ${CMAKE_INSTALL_LIBDIR})
message("CMAKE_INSTALL_INCLUDEDIR      : " ${CMAKE_INSTALL_INCLUDEDIR})

# print environment information
message("CMAKE_SYSTEM_NAME             : " ${CMAKE_SYSTEM_NAME})
message("CMAKE_SYSTEM_PROCESSOR        : " ${CMAKE_SYSTEM_PROCESSOR})
message("CMAKE_CXX_COMPILER_ID         : " ${CMAKE_C_COMPILER_ID})

# cpu architecture detection
string(TOLOWER ${CMAKE_SYSTEM_PROCESSOR} CPU)
set(CPU_X86 x86 i386 i686 x86_64 amd64)
set(CPU_ARM armv6l armv7l arm64 aarch64)
list(FIND CPU_X86 ${CPU} X86_MATCH)
list(FIND CPU_ARM ${CPU} ARM_MATCH)
if(X86_MATCH GREATER "-1")
    set(X86 1)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(X86_64 1)
        set(ARCH_X86_64 1)
        message(STATUS "Detected x86_64 target processor")
    else()
        set(ARCH_X86 1)
        message(STATUS "Detected x86 target processor")
    endif()
elseif(ARM_MATCH GREATER "-1")
    set(ARM 1)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(ARM64 1)
        set(ARCH_AARCH64 1)
        message(STATUS "Detected ARM64 target processor")
    else()
        set(ARCH_ARM 1)
        message(STATUS "Detected ARM target processor")
    endif()
else()
    message(FATAL_ERROR "Unknown target processor: ${CMAKE_SYSTEM_PROCESSOR}.")
endif()

# platform detection
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(SYS_LINUX 1)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(SYS_WINDOWS 1)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(SYS_MACOSX 1)
else()
    message(FATAL_ERROR "Detected unknown target platform: " ${CMAKE_SYSTEM_NAME})
endif()

# compiler detection
if(CMAKE_C_COMPILER_ID STREQUAL "Clang" OR CMAKE_C_COMPILER_ID STREQUAL "AppleClang")
    set(CLANG 1)
endif()
if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
    set(GCC 1)
endif()

# Architecture-specific compiler flags
include(ArchSpecificFlags)

# NASM assembler for x86/x86_64
if(ENABLE_ASM AND (X86 OR X86_64))
    enable_language(ASM_NASM)
    if(NOT CMAKE_ASM_NASM_COMPILER)
        message(WARNING "NASM assembler not found, disabling assembly optimizations")
        set(ENABLE_ASM OFF)
    else()
        message(STATUS "NASM assembler found: ${CMAKE_ASM_NASM_COMPILER}")
    endif()
endif()

# External libraries
include(X264ExternalLibs)

# set config.h and x264_config.h
include(X264Config)
set(X264_CONFIG_H ${PROJECT_BINARY_DIR}/config.h)
set(X264_EXPORT_CONFIG_H ${PROJECT_BINARY_DIR}/x264_config.h)
configure_file(${PROJECT_SOURCE_DIR}/config.h.in ${X264_CONFIG_H} @ONLY)
configure_file(${PROJECT_SOURCE_DIR}/x264_config.h.in ${X264_EXPORT_CONFIG_H} @ONLY)

# set C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED TRUE)
set(CMAKE_C_EXTENSIONS ON)

# Accumulate compile definitions/options for all targets
set(X264_COMPILE_DEFINITIONS HAVE_CONFIG_H)

# set compile options
if(MSVC)
    list(APPEND X264_COMPILE_DEFINITIONS _CRT_SECURE_NO_WARNINGS HAVE_STRING_H)
    x264_append_compile_options(/W3 /Ob2 /Oi /Ot /Oy /Gy /MP /fp:fast)

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        list(APPEND X264_COMPILE_DEFINITIONS _DEBUG)
        x264_append_compile_options(/GS) # Enable security check in debug
    else()
        list(APPEND X264_COMPILE_DEFINITIONS NDEBUG)
        x264_append_compile_options(/GS-) # Disable security check in release for performance
    endif()
else()
    # Platform libraries
    list(APPEND PLATFORM_LIBS m)

    # C standard and source definitions
    if(GCC OR CLANG)
        list(APPEND X264_COMPILE_DEFINITIONS _GNU_SOURCE)
    endif()

    # MinGW specific
    if(MINGW)
        list(APPEND X264_COMPILE_DEFINITIONS _POSIX_C_SOURCE=200112L)
    endif()

    # Warning flags
    x264_append_compile_options(-Wall)
    add_c_flag_if_supported("-Wshadow")
    add_c_flag_if_supported("-Wno-maybe-uninitialized")

    # Debug mode: additional warnings
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        x264_append_compile_options(-Wextra -Wformat=2 -Wfatal-errors)
    endif()

    # Optimization and security flags
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        # Debug mode: enable security checks and keep frame pointer for debugging
        add_c_flag_if_supported("-fstack-protector-strong")
        add_c_flag_if_supported("-ftrapv")
        x264_append_compile_options(-fno-omit-frame-pointer)
    elseif(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
        x264_append_compile_options(-O3 -ffast-math)
        add_c_flag_if_supported("-fomit-frame-pointer")
    endif()

    # Disable tree vectorization (x264 has hand-optimized vectorization)
    add_c_flag_if_supported("-fno-tree-vectorize")

    # Windows x86 GCC workaround
    if(WIN32 AND X86 AND GCC)
        add_c_flag_if_supported("-fno-zero-initialized-in-bss")
    endif()

    # Symbol visibility
    if(GCC OR CLANG)
        add_c_flag_if_supported("-fvisibility=hidden")
    endif()

    # LTO
    if(ENABLE_LTO)
        add_c_flag_if_supported("-flto")
        add_linker_flag_if_supported("-O3" SHARED)
        add_linker_flag_if_supported("-flto" SHARED)
        add_linker_flag_if_supported("-O3" EXE)
        add_linker_flag_if_supported("-flto" EXE)
    endif()

    # Address sanitizer
    if(ENABLE_ASAN)
        x264_append_compile_options(-fsanitize=address -fno-omit-frame-pointer)
        x264_append_link_options(BOTH -fsanitize=address)
    endif()

    # Platform-specific linker flags
    if(BUILD_SHARED_LIBS AND NOT WIN32)
        if(GCC AND SYS_LINUX)
            add_linker_flag_if_supported("-Wl,-Bsymbolic" SHARED)
            x264_append_link_options(SHARED -Wl,-z,now,-z,relro,-z,noexecstack)
            x264_append_link_options(EXE -Wl,-z,now,-z,relro,-z,noexecstack)
        endif()
    endif()

    if(SYS_MACOSX)
        x264_append_compile_options(-fno-common)
        if(NOT BUILD_SHARED_LIBS)
            add_c_flag_if_supported("-mdynamic-no-pic")
        endif()
    endif()
endif()

# Apply user-provided C_FLAGS last
if(DEFINED C_FLAGS AND C_FLAGS)
    separate_arguments(_user_flags NATIVE_COMMAND "${C_FLAGS}")
    if(_user_flags)
        x264_append_compile_options(${_user_flags})
    endif()
endif()

get_property(X264_COMPILE_OPTIONS GLOBAL PROPERTY X264_COMPILE_OPTION_LIST)
get_property(X264_SHARED_LINK_OPTIONS GLOBAL PROPERTY X264_SHARED_LINK_OPTION_LIST)
get_property(X264_EXE_LINK_OPTIONS GLOBAL PROPERTY X264_EXE_LINK_OPTION_LIST)
list(REMOVE_DUPLICATES X264_COMPILE_OPTIONS)
list(REMOVE_DUPLICATES X264_SHARED_LINK_OPTIONS)
list(REMOVE_DUPLICATES X264_EXE_LINK_OPTIONS)
list(REMOVE_DUPLICATES X264_COMPILE_DEFINITIONS)

add_library(x264_options INTERFACE)
target_compile_features(x264_options INTERFACE c_std_99)
target_include_directories(x264_options INTERFACE
    $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}>
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/common>
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/encoder>
)
if(X264_COMPILE_DEFINITIONS)
    foreach(def IN LISTS X264_COMPILE_DEFINITIONS)
        target_compile_definitions(x264_options INTERFACE $<$<COMPILE_LANGUAGE:C>:${def}>)
    endforeach()
endif()
if(X264_COMPILE_OPTIONS)
    foreach(opt IN LISTS X264_COMPILE_OPTIONS)
        target_compile_options(x264_options INTERFACE $<$<COMPILE_LANGUAGE:C>:${opt}>)
    endforeach()
endif()

if(HAS_OPENCL AND SYS_LINUX)
    list(APPEND PLATFORM_LIBS dl)
endif()

list(REMOVE_DUPLICATES PLATFORM_LIBS)

# and subdirectories
add_subdirectory(common)
add_subdirectory(encoder)

# Collect all object files for x264 library
set(X264_OBJECTS)

# Add API layer (bit-depth independent)
if(HAS_ENCODER_API)
    list(APPEND X264_OBJECTS $<TARGET_OBJECTS:encoder_api>)
endif()

# Add encoder objects
if(HAS_ENCODER_8)
    list(APPEND X264_OBJECTS $<TARGET_OBJECTS:encoder_8>)
endif()
if(HAS_ENCODER_10)
    list(APPEND X264_OBJECTS $<TARGET_OBJECTS:encoder_10>)
endif()

# Add common base objects (bit-depth independent)
if(HAS_COMMON_BASE)
    list(APPEND X264_OBJECTS $<TARGET_OBJECTS:common_base>)
endif()
if(HAS_COMMON_SHARED)
    list(APPEND X264_OBJECTS $<TARGET_OBJECTS:${COMMON_SHARED_TARGET}>)
endif()

# Add common objects
if(HAS_COMMON_8)
    list(APPEND X264_OBJECTS $<TARGET_OBJECTS:common_8>)
endif()
if(HAS_COMMON_10)
    list(APPEND X264_OBJECTS $<TARGET_OBJECTS:common_10>)
endif()

# Add assembly objects if available
if(HAS_ASM_TARGET AND ASM_TARGET_NAMES)
    set(_asm_targets_included FALSE)
    foreach(_asm_target ${ASM_TARGET_NAMES})
        if(TARGET ${_asm_target})
            message(STATUS "Including assembly optimizations from target: ${_asm_target}")
            list(APPEND X264_OBJECTS $<TARGET_OBJECTS:${_asm_target}>)
            set(_asm_targets_included TRUE)
        endif()
    endforeach()
    if(NOT _asm_targets_included)
        message(STATUS "No assembly targets were generated despite ENABLE_ASM=ON")
    endif()
else()
    message(STATUS "Building without assembly optimizations")
endif()

# Create x264 library
if(NOT X264_OBJECTS)
    message(FATAL_ERROR "No object files to build x264 library")
endif()

add_library(x264 ${X264_OBJECTS})
if(WIN32 AND BUILD_SHARED_LIBS)
    target_sources(x264 PRIVATE x264dll.c)
endif()
set_target_properties(x264 PROPERTIES
    SOVERSION ${X264_BUILD}
)
target_link_libraries(x264 PUBLIC ${PLATFORM_LIBS})
target_link_libraries(x264 PRIVATE x264_options)
if(X264_SHARED_LINK_OPTIONS)
    target_link_options(x264 PRIVATE ${X264_SHARED_LINK_OPTIONS})
endif()
if(BUILD_SHARED_LIBS)
    target_compile_definitions(x264 PRIVATE X264_API_EXPORTS)
endif()

# Link threading library if found
if(HAVE_THREAD AND Threads_FOUND)
    target_link_libraries(x264 PUBLIC Threads::Threads)
endif()

if(ENABLE_CLI)
    set(X264_CLI_BASE_SRCS
        x264.c
        autocomplete.c
        input/input.c
        input/timecode.c
        input/raw.c
        input/y4m.c
        output/raw.c
        output/matroska.c
        output/matroska_ebml.c
        output/flv.c
        output/flv_bytestream.c
        filters/filters.c
        filters/video/video.c
        filters/video/source.c
        filters/video/internal.c
        filters/video/resize.c
        filters/video/fix_vfr_pts.c
        filters/video/select_every.c
        filters/video/crop.c
    )
    set(X264_CLI_BIT_SRCS
        filters/video/cache.c
        filters/video/depth.c
    )
    if(HAVE_AVS)
        list(APPEND X264_CLI_BASE_SRCS input/avs.c)
    endif()
    if(HAVE_LAVF)
         list(APPEND X264_CLI_BASE_SRCS input/lavf.c)
    endif()
    if(HAVE_FFMS)
         list(APPEND X264_CLI_BASE_SRCS input/ffms.c)
    endif()
    if(HAVE_GPAC)
         list(APPEND X264_CLI_BASE_SRCS output/mp4.c)
    endif()
    if(HAVE_LSMASH)
         list(APPEND X264_CLI_BASE_SRCS output/mp4_lsmash.c)
    endif()
    if(HAVE_THREAD)
        list(APPEND X264_CLI_BIT_SRCS input/thread.c)
    endif()
    if(WIN32)
        list(APPEND X264_CLI_BASE_SRCS extras/getopt.c)
    endif()

    add_library(x264cli_common OBJECT ${X264_CLI_BASE_SRCS})
    target_include_directories(x264cli_common PRIVATE ${PROJECT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
    if(BUILD_SHARED_LIBS)
        target_compile_definitions(x264cli_common PRIVATE X264_API_IMPORTS)
    endif()
    if(X264CLI_OPTIONAL_INCLUDE_DIRS)
        target_include_directories(x264cli_common PRIVATE ${X264CLI_OPTIONAL_INCLUDE_DIRS})
    endif()
    if(X264CLI_OPTIONAL_COMPILE_OPTIONS)
        target_compile_options(x264cli_common PRIVATE ${X264CLI_OPTIONAL_COMPILE_OPTIONS})
    endif()
    target_link_libraries(x264cli_common PRIVATE x264_options)

    set(X264_CLI_OBJECTS $<TARGET_OBJECTS:x264cli_common>)

    if(X264_CLI_BIT_SRCS)
        if(HAVE_BITDEPTH8)
            add_library(x264cli_8 OBJECT ${X264_CLI_BIT_SRCS})
            target_compile_definitions(x264cli_8 PRIVATE HIGH_BIT_DEPTH=0 BIT_DEPTH=8)
            target_include_directories(x264cli_8 PRIVATE ${PROJECT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
            if(X264CLI_OPTIONAL_INCLUDE_DIRS)
                target_include_directories(x264cli_8 PRIVATE ${X264CLI_OPTIONAL_INCLUDE_DIRS})
            endif()
            if(X264CLI_OPTIONAL_COMPILE_OPTIONS)
                target_compile_options(x264cli_8 PRIVATE ${X264CLI_OPTIONAL_COMPILE_OPTIONS})
            endif()
            if(BUILD_SHARED_LIBS)
                target_compile_definitions(x264cli_8 PRIVATE X264_API_IMPORTS)
            endif()
            target_link_libraries(x264cli_8 PRIVATE x264_options)
            list(APPEND X264_CLI_OBJECTS $<TARGET_OBJECTS:x264cli_8>)
        endif()
        if(HAVE_BITDEPTH10)
            add_library(x264cli_10 OBJECT ${X264_CLI_BIT_SRCS})
            target_compile_definitions(x264cli_10 PRIVATE HIGH_BIT_DEPTH=1 BIT_DEPTH=10)
            target_include_directories(x264cli_10 PRIVATE ${PROJECT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
            if(X264CLI_OPTIONAL_INCLUDE_DIRS)
                target_include_directories(x264cli_10 PRIVATE ${X264CLI_OPTIONAL_INCLUDE_DIRS})
            endif()
            if(X264CLI_OPTIONAL_COMPILE_OPTIONS)
                target_compile_options(x264cli_10 PRIVATE ${X264CLI_OPTIONAL_COMPILE_OPTIONS})
            endif()
            if(BUILD_SHARED_LIBS)
                target_compile_definitions(x264cli_10 PRIVATE X264_API_IMPORTS)
            endif()
            target_link_libraries(x264cli_10 PRIVATE x264_options)
            list(APPEND X264_CLI_OBJECTS $<TARGET_OBJECTS:x264cli_10>)
        endif()
    endif()

    add_executable(x264cli)
    target_sources(x264cli PRIVATE ${X264_CLI_OBJECTS})
    target_link_libraries(x264cli PRIVATE x264 x264_options)
    set_target_properties(x264cli PROPERTIES OUTPUT_NAME x264)
    target_include_directories(x264cli PRIVATE ${PROJECT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
    if(BUILD_SHARED_LIBS)
        target_compile_definitions(x264cli PRIVATE X264_API_IMPORTS)
    endif()
    if(X264CLI_OPTIONAL_INCLUDE_DIRS)
        target_include_directories(x264cli PRIVATE ${X264CLI_OPTIONAL_INCLUDE_DIRS})
    endif()
    if(X264CLI_OPTIONAL_LIBS)
        target_link_libraries(x264cli PRIVATE ${X264CLI_OPTIONAL_LIBS})
    endif()
    if(X264_EXE_LINK_OPTIONS)
        target_link_options(x264cli PRIVATE ${X264_EXE_LINK_OPTIONS})
    endif()

    set(CMAKE_SKIP_RPATH FALSE)
    set_target_properties(x264cli PROPERTIES
        INSTALL_RPATH ${CMAKE_INSTALL_LIBDIR}
        BUILD_WITH_INSTALL_RPATH TRUE
    )
    install(TARGETS x264cli RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()

set(_x264_dep_libs ${PLATFORM_LIBS})
if(HAVE_THREAD AND Threads_FOUND AND CMAKE_THREAD_LIBS_INIT)
    list(APPEND _x264_dep_libs ${CMAKE_THREAD_LIBS_INIT})
endif()
list(REMOVE_DUPLICATES _x264_dep_libs)
string(REPLACE ";" " " X264_PC_DEP_LIBS "${_x264_dep_libs}")

set(X264_PC_LIBS_SUFFIX "")
set(X264_PC_LIBS_PRIVATE "")
if(X264_PC_DEP_LIBS)
    if(BUILD_SHARED_LIBS)
        set(X264_PC_LIBS_PRIVATE " ${X264_PC_DEP_LIBS}")
    else()
        set(X264_PC_LIBS_SUFFIX " ${X264_PC_DEP_LIBS}")
    endif()
endif()

set(X264_PC_CFLAGS "")
if(BUILD_SHARED_LIBS)
    set(X264_PC_CFLAGS " -DX264_API_IMPORTS")
endif()

configure_file(${PROJECT_SOURCE_DIR}/cmake/x264.pc.in ${PROJECT_BINARY_DIR}/x264.pc @ONLY)

install(TARGETS x264
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
install(FILES ${PROJECT_SOURCE_DIR}/x264.h ${X264_EXPORT_CONFIG_H} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(FILES ${PROJECT_BINARY_DIR}/x264.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
