#include <catch2/catch_test_macros.hpp>

#include "WaveGraph/WeaveGraphManager.hpp"

namespace Mirage::EditorCore::WaveGraph {
namespace {

TEST_CASE("WeaveGraphManager creates graphs with sequential ids", "[editor-core][unit][wave-graph-manager]") {
    WeaveGraphManager manager;

    const WeaveGraphId firstGraphId = manager.CreateGraph();
    const WeaveGraphId secondGraphId = manager.CreateGraph();

    REQUIRE(firstGraphId == 1);
    REQUIRE(secondGraphId == 2);
}

TEST_CASE("WeaveGraphManager stores created graphs and returns the same instance", "[editor-core][unit][wave-graph-manager]") {
    WeaveGraphManager manager;

    const WeaveGraphId graphId = manager.CreateGraph();
    const auto graph = manager.GetGraph(graphId);

    REQUIRE(graph != nullptr);
    REQUIRE(manager.ContainsGraph(graphId));
    REQUIRE(manager.GetGraph(graphId) == graph);
}

TEST_CASE("WeaveGraphManager returns null for an unknown graph id", "[editor-core][unit][wave-graph-manager]") {
    WeaveGraphManager manager;

    REQUIRE_FALSE(manager.ContainsGraph(42));
    REQUIRE(manager.GetGraph(42) == nullptr);
}

TEST_CASE("WeaveGraphManager deletes existing graphs and reports missing ones", "[editor-core][unit][wave-graph-manager]") {
    WeaveGraphManager manager;

    const WeaveGraphId graphId = manager.CreateGraph();

    REQUIRE(manager.DeleteGraph(graphId));
    REQUIRE_FALSE(manager.ContainsGraph(graphId));
    REQUIRE(manager.GetGraph(graphId) == nullptr);
    REQUIRE_FALSE(manager.DeleteGraph(graphId));
}

} // namespace
} // namespace Mirage::EditorCore::WaveGraph
