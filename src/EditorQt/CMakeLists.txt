find_package(Qt6 REQUIRED COMPONENTS Widgets OpenGL OpenGLWidgets)

qt_standard_project_setup()

set(WeaveCanvasSources
        WeaveCanvasBase/CanvasBaseInputController.cpp
        ${CMAKE_SOURCE_DIR}/include/EditorQt/public/WeaveCanvasBase/WeaveCanvasBase.h WeaveCanvasBase/WeaveCanvasBase.cpp
        WeaveCanvasBase/WeaveQuadTree.cpp
)

set(EditorSources
    Editor/Grid/WeaveEditorGrid.cpp
        ${CMAKE_SOURCE_DIR}/include/EditorQt/public/Editor/RenderedWeaveNodeItem.h Editor/RenderedWeaveNodeItem.cpp
)

set(EDITORS_SOURCES
    Commands/EditorCommandManager.cpp
    Commands/MoveRenderedItemsCommand.cpp
    Editor/WeaveCanvas/WeaveColorPicker.cpp
    Editor/WeaveCanvas/WeaveCanvas.cpp
    Editor/WeaveCanvas/Command/WeaveCanvasMoveNodesCommand.cpp
    Editor/WeaveCanvas/WeaveCanvasQuadTree.cpp
    Editor/WeaveCanvas/WeaveNodePainter.cpp
    Editor/WeaveCanvas/WeaveEditor.cpp
    Editor/SearchNodesModalWidget/SearchNodesModalWidget.cpp
    ${CMAKE_SOURCE_DIR}/include/EditorQt/public/Commands/EditorCommandManager.hpp
    ${CMAKE_SOURCE_DIR}/include/EditorQt/public/Commands/IEditorCommand.hpp
    ${CMAKE_SOURCE_DIR}/include/EditorQt/public/Commands/MoveRenderedItemsCommand.hpp
    ${CMAKE_SOURCE_DIR}/include/EditorQt/public/Editor/WeaveCanvas/WeaveColorPicker.h
    ${CMAKE_SOURCE_DIR}/include/EditorQt/public/Editor/WeaveCanvas/WeaveCanvas.h
    ${CMAKE_SOURCE_DIR}/include/EditorQt/public/Editor/WeaveCanvas/Command/WeaveCanvasMovementSession.h
    ${CMAKE_SOURCE_DIR}/include/EditorQt/public/Editor/WeaveCanvas/Command/WeaveCanvasMoveNodesCommand.h
    ${CMAKE_SOURCE_DIR}/include/EditorQt/public/Editor/WeaveCanvas/WeaveCanvasQuadTree.h
    ${CMAKE_SOURCE_DIR}/include/EditorQt/public/Editor/WeaveCanvas/WeaveGridPainter.h
    ${CMAKE_SOURCE_DIR}/include/EditorQt/public/Editor/WeaveCanvas/WeaveNodePainter.h
    ${CMAKE_SOURCE_DIR}/include/EditorQt/public/Editor/WeaveCanvas/WeaveEditor.h
    ${CMAKE_SOURCE_DIR}/include/EditorQt/public/Editor/Grid/WeaveEditorGrid.h
    ${CMAKE_SOURCE_DIR}/include/EditorQt/public/Editor/SearchNodesModalWidget/SearchNodesModalWidget.hpp
    ${CMAKE_SOURCE_DIR}/include/EditorQt/public/WeaveCanvasBase/CanvasBaseInputController.h
    ${CMAKE_SOURCE_DIR}/include/EditorQt/public/WeaveCanvasBase/CanvasCamera.h
    ${CMAKE_SOURCE_DIR}/include/EditorQt/public/WeaveCanvasBase/WeaveQuadTree.h
    ${CMAKE_SOURCE_DIR}/include/EditorQt/public/WeaveCanvasBase/Grid/ICanvasGrid.h

    ${WeaveCanvasSources}
    ${EditorSources}
)

qt_add_resources(ENGINE_RESOURCES resources.qrc)
qt_add_executable(MirageEngineEditor
    Docking/DockingTab.cpp
    Docking/DockingTabBar.cpp
    Docking/MainDockingWindow.cpp
    Docking/DockingWorkspacePage.cpp
    Docking/DockingWidget.cpp
    Docking/DockingWindow.cpp
    Docking/Demo/InspectorDockingWidget.cpp
    Docking/Demo/SceneDockingWidget.cpp
    Explorer/ExplorerFileWidget.cpp
    Explorer/ExplorerFileEntry.cpp
    Explorer/ExplorerFolderWidget.cpp
    Explorer/ExplorerFolderEntry.cpp
    Explorer/ExplorerEntry.cpp
    Explorer/ExplorerItemWidget.cpp
    Explorer/ExplorerWidget.cpp
    EditorTitleBar/EditorTitleBar.cpp
    ${CMAKE_SOURCE_DIR}/include/EditorQt/public/Docking/DockingTab.hpp
    ${CMAKE_SOURCE_DIR}/include/EditorQt/public/Docking/DockingTabBar.hpp
    ${CMAKE_SOURCE_DIR}/include/EditorQt/public/Docking/DockingWorkspacePage.hpp
    ${CMAKE_SOURCE_DIR}/include/EditorQt/public/Docking/MainDockingWindow.hpp
    ${CMAKE_SOURCE_DIR}/include/EditorQt/public/Docking/DockingWidget.hpp
    ${CMAKE_SOURCE_DIR}/include/EditorQt/public/Docking/DockingWindow.hpp
    ${CMAKE_SOURCE_DIR}/include/EditorQt/public/Docking/Demo/InspectorDockingWidget.hpp
    ${CMAKE_SOURCE_DIR}/include/EditorQt/public/Docking/Demo/SceneDockingWidget.hpp
    ${CMAKE_SOURCE_DIR}/include/EditorQt/public/Explorer/ExplorerFileWidget.hpp
    ${CMAKE_SOURCE_DIR}/include/EditorQt/public/Explorer/ExplorerFileEntry.hpp
    ${CMAKE_SOURCE_DIR}/include/EditorQt/public/Explorer/ExplorerFolderWidget.hpp
    ${CMAKE_SOURCE_DIR}/include/EditorQt/public/Explorer/ExplorerFolderEntry.hpp
    ${CMAKE_SOURCE_DIR}/include/EditorQt/public/Explorer/ExplorerEntry.hpp
    ${CMAKE_SOURCE_DIR}/include/EditorQt/public/Explorer/ExplorerItemWidget.hpp
    ${CMAKE_SOURCE_DIR}/include/EditorQt/public/Explorer/ExplorerWidget.hpp
    ${CMAKE_SOURCE_DIR}/include/EditorQt/public/EditorTitleBar/EditorTitleBar.hpp
    ${CMAKE_SOURCE_DIR}/include/EditorQt/public/MainWindow/MainWindow.hpp
    main.cpp
    MainWindow/MainWindow.cpp
    ${ENGINE_RESOURCES} ${EDITORS_SOURCES}
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
        Qt6::OpenGL
        Qt6::OpenGLWidgets
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
