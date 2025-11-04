# CMake script to convert OpenCL sources to C char array
# Cross-platform replacement for cltostr.sh

if(NOT DEFINED OUTPUT_FILE)
    message(FATAL_ERROR "OUTPUT_FILE not defined")
endif()

if(NOT DEFINED INPUT_FILES)
    message(FATAL_ERROR "INPUT_FILES not defined")
endif()

# Read and sanitize all input files
set(SANITIZED_CONTENT "")
foreach(INPUT_FILE ${INPUT_FILES})
    if(NOT EXISTS "${INPUT_FILE}")
        message(FATAL_ERROR "Input file does not exist: ${INPUT_FILE}")
    endif()
    
    file(READ "${INPUT_FILE}" FILE_CONTENT)
    
    # Split into lines
    string(REGEX REPLACE "\r?\n" ";" LINES "${FILE_CONTENT}")
    
    foreach(LINE ${LINES})
        # Remove leading whitespace
        string(REGEX REPLACE "^[ \t]+" "" LINE "${LINE}")
        # Skip empty lines and C++ style comments
        if(LINE AND NOT LINE MATCHES "^//")
            string(APPEND SANITIZED_CONTENT "${LINE}\n")
        endif()
    endforeach()
endforeach()

# Write sanitized content to temporary file for MD5 calculation
set(TEMP_FILE "${OUTPUT_FILE}.temp")
file(WRITE "${TEMP_FILE}" "${SANITIZED_CONTENT}")

# Convert content to hex array
file(READ "${TEMP_FILE}" HEX_CONTENT HEX)

# Build the char array
set(OUTPUT_CONTENT "static const char x264_opencl_source[] = {\n")

string(LENGTH "${HEX_CONTENT}" HEX_LENGTH)
math(EXPR LAST_IDX "${HEX_LENGTH} - 1")

set(BYTE_COUNT 0)
foreach(IDX RANGE 0 ${LAST_IDX} 2)
    string(SUBSTRING "${HEX_CONTENT}" ${IDX} 2 BYTE)
    
    if(BYTE_COUNT GREATER 0)
        string(APPEND OUTPUT_CONTENT ", ")
    endif()
    
    # Add newline every 12 bytes for readability
    math(EXPR MOD_RESULT "${BYTE_COUNT} % 12")
    if(MOD_RESULT EQUAL 0 AND BYTE_COUNT GREATER 0)
        string(APPEND OUTPUT_CONTENT "\n")
    endif()
    
    string(APPEND OUTPUT_CONTENT "0x${BYTE}")
    math(EXPR BYTE_COUNT "${BYTE_COUNT} + 1")
endforeach()

string(APPEND OUTPUT_CONTENT ", 0x00 };\n\n")

# Calculate MD5 hash
file(MD5 "${TEMP_FILE}" MD5_HASH)

# Convert MD5 hash (32 hex chars) to char array
string(APPEND OUTPUT_CONTENT "static const char x264_opencl_source_hash[] = {\n")

set(HASH_BYTE_COUNT 0)
foreach(IDX RANGE 0 30 2)
    string(SUBSTRING "${MD5_HASH}" ${IDX} 2 HASH_BYTE)
    
    if(HASH_BYTE_COUNT GREATER 0)
        string(APPEND OUTPUT_CONTENT ", ")
    endif()
    
    math(EXPR MOD_RESULT "${HASH_BYTE_COUNT} % 12")
    if(MOD_RESULT EQUAL 0 AND HASH_BYTE_COUNT GREATER 0)
        string(APPEND OUTPUT_CONTENT "\n")
    endif()
    
    string(APPEND OUTPUT_CONTENT "0x${HASH_BYTE}")
    math(EXPR HASH_BYTE_COUNT "${HASH_BYTE_COUNT} + 1")
endforeach()

string(APPEND OUTPUT_CONTENT ", 0x00 };\n")

# Write output file
file(WRITE "${OUTPUT_FILE}" "${OUTPUT_CONTENT}")

# Clean up temp file
file(REMOVE "${TEMP_FILE}")

message(STATUS "Generated ${OUTPUT_FILE}")
