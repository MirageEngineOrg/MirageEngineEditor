find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_standard_project_setup()


qt_add_resources(ENGINE_RESOURCES resources.qrc)
qt_add_executable(MirageEngineEditor
    Docking/DockingTab.cpp
    Docking/DockingTabBar.cpp
    Docking/DockingWidget.cpp
    Docking/DockingWindow.cpp
    Docking/Demo/InspectorDockingWidget.cpp
    Docking/Demo/SceneDockingWidget.cpp
    EditorTitleBar/EditorTitleBar.cpp
    ${CMAKE_SOURCE_DIR}/include/EditorQt/public/Docking/DockingTab.hpp
    ${CMAKE_SOURCE_DIR}/include/EditorQt/public/Docking/DockingTabBar.hpp
    ${CMAKE_SOURCE_DIR}/include/EditorQt/public/Docking/DockingWidget.hpp
    ${CMAKE_SOURCE_DIR}/include/EditorQt/public/Docking/DockingWindow.hpp
    ${CMAKE_SOURCE_DIR}/include/EditorQt/public/Docking/Demo/InspectorDockingWidget.hpp
    ${CMAKE_SOURCE_DIR}/include/EditorQt/public/Docking/Demo/SceneDockingWidget.hpp
    ${CMAKE_SOURCE_DIR}/include/EditorQt/public/EditorTitleBar/EditorTitleBar.hpp
    ${CMAKE_SOURCE_DIR}/include/EditorQt/public/MainWindow/MainWindow.hpp
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
