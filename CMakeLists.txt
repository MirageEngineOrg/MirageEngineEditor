cmake_minimum_required(VERSION 3.20)

project(MirageEngineEditor VERSION 0.1.0 LANGUAGES CXX)

include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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

find_program(CLANG_TIDY_EXE NAMES clang-tidy)

function(mirage_configure_target target_name)
    if(CLANG_TIDY_EXE)
        set_target_properties(${target_name} PROPERTIES
            CXX_CLANG_TIDY "${CLANG_TIDY_EXE}"
        )
    endif()
endfunction()

add_subdirectory(src/EditorCore)
add_subdirectory(src/EditorQt)
