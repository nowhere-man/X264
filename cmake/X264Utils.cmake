include(CheckCCompilerFlag)

# Internal helper storing compiler/linker feature detections
set_property(GLOBAL PROPERTY X264_COMPILE_OPTION_LIST "")
set_property(GLOBAL PROPERTY X264_SHARED_LINK_OPTION_LIST "")
set_property(GLOBAL PROPERTY X264_EXE_LINK_OPTION_LIST "")

# Append helper functions for compile and linker options
function(x264_append_compile_options)
    if(NOT ARGN)
        return()
    endif()
    get_property(opts GLOBAL PROPERTY X264_COMPILE_OPTION_LIST)
    list(APPEND opts ${ARGN})
    set_property(GLOBAL PROPERTY X264_COMPILE_OPTION_LIST "${opts}")
endfunction()

function(x264_append_link_options kind)
    set(flags ${ARGN})
    if(NOT flags)
        return()
    endif()
    if(kind STREQUAL "SHARED")
        get_property(opts GLOBAL PROPERTY X264_SHARED_LINK_OPTION_LIST)
        list(APPEND opts ${flags})
        set_property(GLOBAL PROPERTY X264_SHARED_LINK_OPTION_LIST "${opts}")
    elseif(kind STREQUAL "EXE")
        get_property(opts GLOBAL PROPERTY X264_EXE_LINK_OPTION_LIST)
        list(APPEND opts ${flags})
        set_property(GLOBAL PROPERTY X264_EXE_LINK_OPTION_LIST "${opts}")
    elseif(kind STREQUAL "BOTH")
        x264_append_link_options("SHARED" ${flags})
        x264_append_link_options("EXE" ${flags})
    else()
        message(FATAL_ERROR "Unknown link option list kind: ${kind}")
    endif()
endfunction()

# Check if the C compiler supports the given flag and append it to the
# global compile option list when supported.
function(add_c_flag_if_supported flag)
    string(REGEX REPLACE "[^A-Za-z0-9]" "_" flag_var "${flag}")
    string(TOUPPER "${flag_var}" flag_var)
    set(cache_var "X264_HAS_${flag_var}")

    check_c_compiler_flag("${flag}" ${cache_var})

    if(${cache_var})
        get_property(opts GLOBAL PROPERTY X264_COMPILE_OPTION_LIST)
        list(APPEND opts "${flag}")
        set_property(GLOBAL PROPERTY X264_COMPILE_OPTION_LIST "${opts}")
    endif()
endfunction()


# Check if the C compiler supports the given flag and append it to the requested
# linker option list ("SHARED" or "EXE") when supported.
function(add_linker_flag_if_supported flag kind)
    string(REGEX REPLACE "[^A-Za-z0-9]" "_" flag_var "${flag}")
    string(TOUPPER "${flag_var}" flag_var)
    set(cache_var "X264_HAS_LINK_${flag_var}")

    check_c_compiler_flag("${flag}" ${cache_var})

    if(${cache_var})
        if(kind STREQUAL "SHARED")
            x264_append_link_options("SHARED" "${flag}")
        elseif(kind STREQUAL "EXE")
            x264_append_link_options("EXE" "${flag}")
        elseif(kind STREQUAL "BOTH")
            x264_append_link_options("BOTH" "${flag}")
        else()
            message(FATAL_ERROR "Unknown linker option list kind: ${kind}")
        endif()
    endif()
endfunction()
