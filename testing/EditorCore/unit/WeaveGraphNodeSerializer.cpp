#include "WaveGraph/Serializer/WeaveGraphSerializer.h"
#include "WaveGraph/WeaveGraph.hpp"
#include "catch2/catch_test_macros.hpp"

namespace Mirage::EditorCore::WaveGraph {

TEST_CASE("WeaveGraphManagerNodeSerializer", "[editor-core][unit][wave-graph-manager]") {

    std::shared_ptr<WeaveGraph>  graph = std::make_shared<WeaveGraph>();
    graph->CreateFunctionStatementNode("MyFunc", {}, {.type=MValueTypes::VOID}, {});

    WeaveGraphSerializer::GetInstance().Serialize("D:\\Projects\\MirageProject\\MirageEngineEditor\\output", graph);
}

}
