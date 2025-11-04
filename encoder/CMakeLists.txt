# Bit-depth dependent encoder sources
set(SRCS_X
    analyse.c
    me.c
    ratecontrol.c
    set.c
    macroblock.c
    cabac.c
    cavlc.c
    encoder.c
    lookahead.c
)

# OpenCL sources (8-bit only)
set(SRCS_8)
if(ENABLE_OPENCL)
    list(APPEND SRCS_8 slicetype-cl.c)
endif()

# api.c should be compiled separately without bit-depth suffix
# It will be added to the main library directly
set(ENCODER_API_SRC api.c PARENT_SCOPE)
set(ENCODER_SRCS_X ${SRCS_X} PARENT_SCOPE)
set(ENCODER_SRCS_8 ${SRCS_8} PARENT_SCOPE)

# Build object libraries for each bit depth (WITHOUT api.c)
if(HAVE_BITDEPTH8)
    add_library(encoder_8 OBJECT ${SRCS_X} ${SRCS_8})
    target_compile_definitions(encoder_8 PRIVATE HIGH_BIT_DEPTH=0 BIT_DEPTH=8)
    target_include_directories(encoder_8 PUBLIC ${PROJECT_BINARY_DIR} ${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/encoder)
    if(BUILD_SHARED_LIBS)
        target_compile_definitions(encoder_8 PRIVATE X264_API_EXPORTS)
    endif()
    target_link_libraries(encoder_8 PRIVATE x264_options)
    set(HAS_ENCODER_8 TRUE PARENT_SCOPE)
endif()

if(HAVE_BITDEPTH10)
    add_library(encoder_10 OBJECT ${SRCS_X})
    target_compile_definitions(encoder_10 PRIVATE HIGH_BIT_DEPTH=1 BIT_DEPTH=10)
    target_include_directories(encoder_10 PUBLIC ${PROJECT_BINARY_DIR} ${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/encoder)
    if(BUILD_SHARED_LIBS)
        target_compile_definitions(encoder_10 PRIVATE X264_API_EXPORTS)
    endif()
    target_link_libraries(encoder_10 PRIVATE x264_options)
    set(HAS_ENCODER_10 TRUE PARENT_SCOPE)
endif()

# api.c as a separate object library
add_library(encoder_api OBJECT api.c)
target_include_directories(encoder_api PUBLIC ${PROJECT_BINARY_DIR} ${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/encoder)
if(BUILD_SHARED_LIBS)
    target_compile_definitions(encoder_api PRIVATE X264_API_EXPORTS)
endif()
target_link_libraries(encoder_api PRIVATE x264_options)

if(HAVE_BITDEPTH8)
    target_compile_definitions(encoder_api PRIVATE HIGH_BIT_DEPTH=0 BIT_DEPTH=8)
elseif(HAVE_BITDEPTH10)
    target_compile_definitions(encoder_api PRIVATE HIGH_BIT_DEPTH=1 BIT_DEPTH=10)
endif()

set(HAS_ENCODER_API TRUE PARENT_SCOPE)
