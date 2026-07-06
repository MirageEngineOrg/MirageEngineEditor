find_package(MirageEngine REQUIRED)

set(EditorCore_WeaveNodesSources
    WaveGraph/Nodes/Expressions/WeaveExpressionNode.cpp

    WaveGraph/Nodes/Expressions/FunctionCallExpression.cpp
        WaveGraph/Nodes/Expressions/BinOps/WeaveBinaryOperationNode.cpp
        WaveGraph/Nodes/Expressions/BinOps/WeaveBaseNumericOp.cpp
        WaveGraph/Nodes/Expressions/BinOps/ComparisonOp.cpp
        WaveGraph/Nodes/Expressions/BinOps/BitOperationOp.cpp
        WaveGraph/Nodes/Expressions/Constrants/WeaveConstantNode.cpp
        WaveGraph/Nodes/Expressions/Variables/GetLocalVariableExpression.cpp
        WaveGraph/Nodes/Expressions/Variables/GetClassVariableExpression.cpp

        WaveGraph/Nodes/Statements/StatementNode.cpp
        WaveGraph/Nodes/Statements/SetLocalVariableStatementNode.cpp
        WaveGraph/Nodes/Statements/FunctionStatementNode.cpp

        WaveGraph/Serializer/ByteCodeFileWriter.cpp

        WaveGraph/Nodes/Statements/ExpressionStatement.cpp
)

set(EditorCore_WeaveSources
        WaveGraph/NodeProvider/WeaveBuiltinNodeProvider.cpp
        ${EditorCore_WeaveNodesSources}
        WaveGraph/WeaveGraph.cpp
        WaveGraph/WeaveGraphManager.cpp
        WaveGraph/WeaveGraphNode.cpp
        WaveGraph/WeaveNodeRegistry.cpp
        WaveGraph/WeaveNodeRegistryTypes.cpp
        WaveGraph/NodeKindHelper.cpp

        WaveGraph/Serializer/WeaveGraphSerializer.cpp
        WaveGraph/Serializer/WeaveGraphBytecodeCollector.cpp
)

add_library(EditorCore STATIC
    Application/EditorCore.cpp
    Application/EditorApplication.cpp

    ${EditorCore_WeaveSources}
)

target_link_libraries(EditorCore PUBLIC MirageEngine::Runtime MirageEngine::CoreMObject)

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
