aux_source_directory(. SRC_LIST)

# remove non-source files
list(REMOVE_ITEM SRC_LIST ./rdo.c ./slicetype.c)

if(NOT ENABLE_OPENCL)
    list(REMOVE_ITEM SRC_LIST ./slicetype-cl.c)
endif()

add_library(encoder OBJECT ${SRC_LIST})

target_include_directories(encoder PUBLIC
    ${PROJECT_SOURCE_DIR}/
    ${PROJECT_SOURCE_DIR}/encoder/
)