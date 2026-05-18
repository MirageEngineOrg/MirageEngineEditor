cmake_minimum_required(VERSION 3.20)

project(MirageEngineEditor VERSION 0.1.0 LANGUAGES CXX)

# Use C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Export compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Clang warnings
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(
        -Wall
        -Wextra
        -Wpedantic
        -Wshadow
        -Wconversion
        -Wsign-conversion
        -Wnull-dereference
        -Wdouble-promotion
    )
endif()

# Executable
add_executable(MirageEngineEditor
   src/main.cpp
)

target_include_directories(MirageEngineEditor
    PRIVATE include
)

# clang-tidy
find_program(CLANG_TIDY_EXE NAMES clang-tidy)

if(CLANG_TIDY_EXE)
	set_target_properties(MirageEngineEditor PROPERTIES
        CXX_CLANG_TIDY "${CLANG_TIDY_EXE}"
    )
endif()

