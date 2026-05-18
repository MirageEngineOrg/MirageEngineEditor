add_library(EditorCore STATIC
    Application/EditorApplication.cpp
)

target_include_directories(EditorCore
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include/EditorCore/public>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include/EditorCore/private>
)

target_compile_features(EditorCore PUBLIC cxx_std_20)

mirage_configure_target(EditorCore)

install(TARGETS EditorCore
    EXPORT MirageEngineEditorTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/EditorCore/public/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
