#pragma once

#include "Nodes/Base/LocalVariable.h"
#include "Nodes/Expressions/Constants/WeaveConstantNode.hpp"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace Mirage::EditorCore::WaveGraph {
class FunctionStatementNode;

class WeaveGraph {
public:
    using NodeMap = std::unordered_map<WeaveNodeId, std::shared_ptr<WeaveGraphNode>>;
    using LocalVarMap = std::unordered_map<LocalVariableId, std::shared_ptr<LocalVariable>>;

    WeaveGraph() = default;

    template<typename NodeValueType>
    std::shared_ptr<WeaveConstantNode> CreateConstantNode(NodeValueType value);

    template<typename T>
    [[nodiscard]] std::shared_ptr<T> CreateNodeByKind(WeaveGraphNodeKind kind);

    std::shared_ptr<LocalVariable> ConstructLocalVariable(const LocalVariableSignature& signature);
    std::shared_ptr<LocalVariable> ConstructorArrayLocalVariable(const std::string& name, const MType& template_type);
    std::shared_ptr<LocalVariable> ConstructorMapLocalVariable(const std::string& name, const MType& key_template_type, const MType& value_template_type);

    std::shared_ptr<FunctionStatementNode> CreateFunctionStatementNode(const std::string &name, const std::vector<LocalVariable> &params,
                                MType return_variable_type,
                                const std::vector<LocalVariable> &local_vars);

    template<typename T, typename... Args>
    [[nodiscard]] std::shared_ptr<T> CreateNode(Args&&... args);

    [[nodiscard]] std::shared_ptr<WeaveGraphNode> GetNode(WeaveNodeId id) const;
    [[nodiscard]] bool ContainsNode(WeaveNodeId id) const noexcept;
    [[nodiscard]] std::size_t GetNodeCount() const noexcept;
    [[nodiscard]] WeaveNodeId PeekNextNodeId() const noexcept;

    std::vector<WeaveNodeId> function_statements;
private:
    [[nodiscard]] WeaveNodeId GenerateNodeId() noexcept;

    NodeMap nodes_ {};
    WeaveNodeId nextNodeId_{1};

    LocalVarMap localVarMap_ {};
    LocalVariableId nextVariableId_{2};

};

template<typename NodeValueType>
std::shared_ptr<WeaveConstantNode> WeaveGraph::CreateConstantNode(NodeValueType value) {
    const WeaveNodeId id = GenerateNodeId();
    auto node = std::make_shared<WeaveConstantNode>(id, std::move(value));
    nodes_.emplace(id, node);
    return node;
}



template <typename T> std::shared_ptr<T> WeaveGraph::CreateNodeByKind(WeaveGraphNodeKind kind) {
    static_assert(std::is_base_of_v<WeaveGraphNode, T>);

    const WeaveNodeId id = GenerateNodeId();
    auto node = std::make_shared<T>(kind,id);
    nodes_.emplace(id, node);

    if (kind == WeaveGraphNodeKinds::FunctionStatement) {
        function_statements.emplace_back(id);
    }

    return node;
}
template <typename T, typename... Args> std::shared_ptr<T> WeaveGraph::CreateNode(Args &&...args) {
    const WeaveNodeId id = GenerateNodeId();
    auto node = std::make_shared<T>(id, std::forward<Args>(args)...);

    if (node->GetKind() == WeaveGraphNodeKinds::FunctionStatement) {
        function_statements.emplace_back(id);
    }

    nodes_.emplace(id, node);
    return node;
}

} // namespace Mirage::EditorCore::WaveGraph
