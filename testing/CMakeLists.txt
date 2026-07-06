include(FetchContent)

option(MIRAGE_FETCH_CATCH2 "Download Catch2 automatically when it is not installed locally" OFF)

find_package(Catch2 3 QUIET CONFIG)

if(NOT Catch2_FOUND)
    if(MIRAGE_FETCH_CATCH2)
        FetchContent_Declare(
            Catch2
            GIT_REPOSITORY https://github.com/catchorg/Catch2.git
            GIT_TAG v3.7.1
        )
        FetchContent_MakeAvailable(Catch2)
    else()
        message(FATAL_ERROR
            "Catch2 v3 was not found. Install Catch2 and make it discoverable via CMAKE_PREFIX_PATH/Catch2_DIR, "
            "or configure with -DMIRAGE_FETCH_CATCH2=ON to download it automatically."
        )
    endif()
endif()

include(Catch)

function(mirage_add_editor_core_test target_name test_kind)
    cmake_parse_arguments(MIRAGE_TEST "" "" "SOURCES" ${ARGN})

    if(NOT MIRAGE_TEST_SOURCES)
        message(FATAL_ERROR "mirage_add_editor_core_test(${target_name} ${test_kind}) requires SOURCES")
    endif()

    if(NOT test_kind MATCHES "^(unit|integration|e2e)$")
        message(FATAL_ERROR
            "mirage_add_editor_core_test(${target_name} ${test_kind}) received unsupported test kind. "
            "Expected one of: unit, integration, e2e."
        )
    endif()

    add_executable(${target_name} ${MIRAGE_TEST_SOURCES})

    target_link_libraries(${target_name}
        PRIVATE
            EditorCore
            Catch2::Catch2WithMain
    )

    target_compile_features(${target_name} PRIVATE cxx_std_20)
    set_target_properties(${target_name} PROPERTIES FOLDER "Tests/EditorCore/${test_kind}")

    mirage_configure_target(${target_name})
    catch_discover_tests(${target_name}
        TEST_PREFIX "EditorCore.${test_kind}::"
        PROPERTIES
            LABELS "editor-core;${test_kind}"
    )
endfunction()

function(mirage_add_editor_core_unit_test target_name)
    mirage_add_editor_core_test(${target_name} unit ${ARGN})
endfunction()

function(mirage_add_editor_core_integration_test target_name)
    mirage_add_editor_core_test(${target_name} integration ${ARGN})
endfunction()

function(mirage_add_editor_core_e2e_test target_name)
    mirage_add_editor_core_test(${target_name} e2e ${ARGN})
endfunction()

add_subdirectory(EditorCore)
