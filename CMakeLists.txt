cmake_minimum_required(VERSION 3.22)
project(PathFinder
    VERSION 1.0.0
    LANGUAGES C
)

# C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Generate compile_commands.json for vscode intellisense
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Default build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose build type" FORCE)
endif()

# Sanitizer options
option(ENABLE_ASAN "Address Sanitizer" OFF)
option(ENABLE_TSAN "Thread Sanitizer" OFF)
option(ENABLE_MSAN "Memory Sanitizer" OFF)

# Check for mutually exclusive sanitizers
if(ENABLE_ASAN AND ENABLE_TSAN)
    message(FATAL_ERROR "Address Sanitizer and Thread Sanitizer cannot be enabled simultaneously")
endif()

if(ENABLE_ASAN AND ENABLE_MSAN)
    message(FATAL_ERROR "Address Sanitizer and Memory Sanitizer cannot be enabled simultaneously")
endif()

if(ENABLE_TSAN AND ENABLE_MSAN)
    message(FATAL_ERROR "Thread Sanitizer and Memory Sanitizer cannot be enabled simultaneously")
endif()

# Apply sanitizers
if(ENABLE_ASAN)
    add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address)
endif()

if(ENABLE_TSAN)
    add_compile_options(-fsanitize=thread)
    add_link_options(-fsanitize=thread)
endif()

if(ENABLE_MSAN)
    add_compile_options(-fsanitize=memory -fPIE -fno-omit-frame-pointer)
    add_link_options(-fsanitize=memory -pie)
endif()

add_subdirectory(external/Logger)
add_subdirectory(external/WIM)

set(RAYLIB_VERSION 5.5)
find_package(raylib ${RAYLIB_VERSION} QUIET) # QUIET or REQUIRED
if (NOT raylib_FOUND)
  include(FetchContent)
  FetchContent_Declare(
    raylib
    DOWNLOAD_EXTRACT_TIMESTAMP OFF
    URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
  )
  FetchContent_GetProperties(raylib)
  if (NOT raylib_POPULATED)
    set(FETCHCONTENT_QUIET NO)
    FetchContent_MakeAvailable(raylib)
  endif()
endif()

# Executable
add_executable(pathfinder 
    src/main.c
)
target_include_directories(pathfinder PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/external/Logger/include
    ${CMAKE_CURRENT_SOURCE_DIR}/external/WIM/include
)
target_link_libraries(pathfinder PRIVATE 
    loggerlib
    wimlib
    raylib
)
target_compile_options(pathfinder PRIVATE 
    -Wall 
    -Wextra 
    -pedantic
)

# Summary
message(STATUS "")
message(STATUS "=======================================================")
message(STATUS "Build:                              ${CMAKE_BUILD_TYPE}")

message(STATUS "C compiler:                         ${CMAKE_C_COMPILER}")
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
message(STATUS "C Debug flags:                      ${CMAKE_C_FLAGS_DEBUG}")
else()
message(STATUS "C Release flags:                    ${CMAKE_C_FLAGS_RELEASE}")
endif()

message(STATUS "Generator:                          ${CMAKE_GENERATOR}")

message(STATUS "Enable Address Sanitizer:           ${ENABLE_ASAN}")
message(STATUS "Enable Thread Sanitizer:            ${ENABLE_TSAN}")
message(STATUS "Enable Memory Sanitizer:            ${ENABLE_MSAN}")
message(STATUS "=======================================================")
message(STATUS "")