cmake_minimum_required(VERSION 3.10)
project(display_gen_wasm C)

# Using C11
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED TRUE)

# Default build type is Release
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build, options are: Debug Release" FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
endif()

# Enables common warnings
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")

# Add all source files to the program
file(GLOB SOURCES "wasm/src/*.c")
add_executable(display_gen ${SOURCES})

# Add include directory
target_include_directories(display_gen PRIVATE
    "wasm/include"
)

# Set compile/link options
set_target_properties(display_gen PROPERTIES
    LINK_FLAGS "\
    -s INITIAL_MEMORY=100663296\
    -s ALLOW_MEMORY_GROWTH\
    -o display_generator.js\
    --use-port=sdl2\
    --use-port=sdl2_image:formats=png\
    --use-port=zlib\
    -s EXPORTED_FUNCTIONS=['_main','_Process_Image','_Process_Init']\
    -s EXPORTED_RUNTIME_METHODS=['addFunction','UTF8ToString']\
    -s ENVIRONMENT=web,worker\
    -s MODULARIZE=1\
    -s EXPORT_ES6=1\
    -s WASM=1\
    --embed-file ${CMAKE_SOURCE_DIR}/assets@/assets\
")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set_property(TARGET display_gen APPEND_STRING PROPERTY LINK_FLAGS " -gsource-map")
endif()
