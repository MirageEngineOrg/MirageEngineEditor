find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_standard_project_setup()


qt_add_resources(ENGINE_RESOURCES resources.qrc)
qt_add_executable(MirageEngineEditor
    EditorTitleBar/EditorTitleBar.cpp
    main.cpp
    MainWindow/MainWindow.cpp
        ${ENGINE_RESOURCES}
)


target_include_directories(MirageEngineEditor
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include/EditorQt/public>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include/EditorQt/private>
)

target_link_libraries(MirageEngineEditor
    PRIVATE
        EditorCore
        Qt6::Widgets
)

target_compile_features(MirageEngineEditor PRIVATE cxx_std_20)

install(TARGETS MirageEngineEditor
    EXPORT MirageEngineEditorTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/EditorQt/public/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
