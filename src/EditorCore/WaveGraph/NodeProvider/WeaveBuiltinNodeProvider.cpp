#include "WaveGraph/NodeProvider/WeaveBuiltinNodeProvider.hpp"

#include "WaveGraph/Nodes/Expressions/BinOps/BitOperationOp.h"
#include "WaveGraph/Nodes/Expressions/BinOps/ComparisonOp.h"
#include "WaveGraph/Nodes/Expressions/BinOps/WeaveBaseNumericOp.h"
#include "WaveGraph/WeaveGraphManager.hpp"

#include <memory>
#include <string>
#include <utility>

namespace Mirage::EditorCore::WaveGraph {

namespace {

WeaveCreatedNodeInfo BuildValueNodeInfo(const WeaveNodeId nodeId, const WeaveGraphNodeKind outputKind) {
    return {
        .nodeId = nodeId,
        .inputs = {},
        .outputs = {
            WeaveNodeOutputDescriptor{
                .name = "value",
                .getKind = [outputKind]() {
                    return outputKind;
                }
            }
        }
    };
}

template<typename ConnectionHandlerType>
WeaveCreatedNodeInfo BinOpNodeInfo(
    const WeaveBinaryOperationNode* node,
    std::unique_ptr<ConnectionHandlerType> input0_handler,
    std::unique_ptr<ConnectionHandlerType> input1_handler)
{
    std::vector<WeaveNodeInputDescriptor> inputs;

    inputs.emplace_back(WeaveNodeInputDescriptor{
        .name = "0",
        .connection_handler = std::move(input0_handler),
        .getKind = [node]() {
            if (node->GetLeftOperand() == nullptr)
                return WeaveValueKindValues::Unknown;

            return node->GetLeftOperand()->GetValueKind();
        }
    });

    inputs.emplace_back(WeaveNodeInputDescriptor{
        .name = "1",
        .connection_handler = std::move(input1_handler),
        .getKind = [node]() {
            if (node->GetRightOperand() == nullptr)
                return WeaveValueKindValues::Unknown;

            return node->GetRightOperand()->GetValueKind();
        }
    });

    std::vector<WeaveNodeOutputDescriptor> outputs;

    outputs.emplace_back(WeaveNodeOutputDescriptor{
        .name = "result",
        .getKind = [node]() {
            return node->GetValueKind();
        }
    });

    return WeaveCreatedNodeInfo{
        .nodeId = node->GetId(),
        .inputs = std::move(inputs),
        .outputs = std::move(outputs)
    };
}

template<typename ValueType>
WeaveNodeInfo CreateConstantNodeInfo(
    const std::string& name,
    const std::string& description,
    const std::shared_ptr<WeaveGraphManager>& graphManager) {
    std::weak_ptr<WeaveGraphManager> weakGraphManager = graphManager;
    return {
        .name = name,
        .description = description,
        .constructor = [weakGraphManager](const WeaveGraphId graphId) -> WeaveCreatedNodeInfo {
            const std::shared_ptr<WeaveGraphManager> graphManagerLock = weakGraphManager.lock();
            if (graphManagerLock == nullptr) {
                return {};
            }

            const std::shared_ptr<WeaveGraph> graph = graphManagerLock->GetGraph(graphId);
            if (graph == nullptr) {
                return {};
            }

            const auto node = graph->CreateConstantNode<ValueType>(ValueType{});
            return BuildValueNodeInfo(node->GetId(), node->GetValueKind());
        }
    };
}

template<typename T, typename InputHandlerType>
WeaveNodeInfo CreateBinNodeByKind(
        WeaveGraphNodeKind kind,
        const std::string& name,
        const std::string& description,
        const std::shared_ptr<WeaveGraphManager>& graphManager
        ) {

    static_assert(std::is_base_of_v<WeaveGraphNode, T>);
    std::weak_ptr<WeaveGraphManager> weakGraphManager = graphManager;
    return {
        .name = name,
        .description = description,
        .constructor = [weakGraphManager,
                            kind](const WeaveGraphId graphId) -> WeaveCreatedNodeInfo {
            const std::shared_ptr<WeaveGraphManager> graphManagerLock = weakGraphManager.lock();
            if (graphManagerLock == nullptr) {
                return {};
            }

            const std::shared_ptr<WeaveGraph> graph = graphManagerLock->GetGraph(graphId);
            if (graph == nullptr) {
                return {};
            }

            const auto node = graph->CreateNodeByKind<T>(kind);
            return BinOpNodeInfo(node.get(), std::make_unique<InputHandlerType>(node, true), std::make_unique<InputHandlerType>(node, true));
        }
    };
}

} // namespace

std::shared_ptr<WeaveBuiltinNodeProvider> WeaveBuiltinNodeProvider::GetInstance() {
    static std::shared_ptr<WeaveBuiltinNodeProvider> instance(new WeaveBuiltinNodeProvider());
    return instance;
}

WeaveNodeGroupInfo WeaveBuiltinNodeProvider::GetNodeGroup(const std::shared_ptr<WeaveGraphManager>& graphManager) const {
    WeaveNodeGroupInfo constantGroup {
        .name = "constant",
        .subGroups = {},
        .nodes = {
            CreateConstantNodeInfo<int64_t>("Integer", "Integer constant", graphManager),
            CreateConstantNodeInfo<float>("Float", "Float constant", graphManager),
            CreateConstantNodeInfo<double>("Double", "Double constant", graphManager),
            CreateConstantNodeInfo<std::string>("String", "String constant", graphManager),
        }
    };

    WeaveNodeGroupInfo numericOpsGroup {
        .name = "Numeric operations",
        .subGroups = {},
        .nodes = {
            CreateBinNodeByKind<WeaveBaseNumericOp, BaseNumericOpInputConnectionHandler>(WeaveGraphNodeKinds::SumOperation, "Sum", "Sums two values", graphManager),
            CreateBinNodeByKind<WeaveBaseNumericOp, BaseNumericOpInputConnectionHandler>(WeaveGraphNodeKinds::SubOperation, "Sub", "Differs two values", graphManager),
            CreateBinNodeByKind<WeaveBaseNumericOp, BaseNumericOpInputConnectionHandler>(WeaveGraphNodeKinds::MulOperation, "Mul", "Multiplies two values", graphManager),
            CreateBinNodeByKind<WeaveBaseNumericOp, BaseNumericOpInputConnectionHandler>(WeaveGraphNodeKinds::DivideOperation, "Div", "Divides one value by another", graphManager),
        }
    };


    WeaveNodeGroupInfo comparisonOpsGroup {
        .name = "Comparison operations",
        .subGroups = {},
        .nodes = {
            CreateBinNodeByKind<ComparisonOp, ComparisonOpInputConnectionHandler>(WeaveGraphNodeKinds::EqualOperation, "Equal", "Checks for equality", graphManager),
            CreateBinNodeByKind<ComparisonOp, ComparisonOpInputConnectionHandler>(WeaveGraphNodeKinds::NotEqualOperation, "NotEqual", "Checks for inequality", graphManager),

            CreateBinNodeByKind<ComparisonOp, ComparisonOpInputConnectionHandler>(WeaveGraphNodeKinds::LessOperation, "Less", "Checks for one value to be less than another", graphManager),
            CreateBinNodeByKind<ComparisonOp, ComparisonOpInputConnectionHandler>(WeaveGraphNodeKinds::LessOrEqualOperation, "LessOrEqual", "Checks for one value to be less or equal than another", graphManager),
            CreateBinNodeByKind<ComparisonOp, ComparisonOpInputConnectionHandler>(WeaveGraphNodeKinds::GreaterOperation, "Greate", "Checks for one value to be greater than another", graphManager),
            CreateBinNodeByKind<ComparisonOp, ComparisonOpInputConnectionHandler>(WeaveGraphNodeKinds::GreaterOrEqualOperation, "GreaterOrEqual", "Checks for one value to be greater or equal than another", graphManager),
        }
    };

    WeaveNodeGroupInfo logicalOpsGroup {
        .name = "Logical operations",
        .subGroups = {},
        .nodes = {
            CreateBinNodeByKind<BitOperationOp, BitOpInputConnectionHandler>(WeaveGraphNodeKinds::LogicalAndOperation, "logical AND (&&)", "Logical AND", graphManager),
            CreateBinNodeByKind<BitOperationOp, BitOpInputConnectionHandler>(WeaveGraphNodeKinds::LogicalOrOperation, "logical OR (||)", "Logical OR", graphManager),
            CreateBinNodeByKind<BitOperationOp, BitOpInputConnectionHandler>(WeaveGraphNodeKinds::BitwiseAndOperation, "bitwise AND (&)", "Bitwise AND", graphManager),
            CreateBinNodeByKind<BitOperationOp, BitOpInputConnectionHandler>(WeaveGraphNodeKinds::BitwiseOrOperation, "bitwise OR (|)", "Bitwise OR", graphManager),
        }
    };

    return {
        .name = "builtin",
        .subGroups = {
            std::move(constantGroup),
            std::move(numericOpsGroup),
            std::move(comparisonOpsGroup),
            std::move(logicalOpsGroup),
        },
        .nodes = {}
    };
}

} // namespace Mirage::EditorCore::WaveGraph
