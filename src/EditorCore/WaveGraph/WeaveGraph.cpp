#include "WaveGraph/WeaveGraph.hpp"

#include "WaveGraph/Nodes/Statements/FunctionStatementNode.h"

#include <utility>
namespace Mirage::EditorCore::WaveGraph {

std::shared_ptr<LocalVariable> WeaveGraph::ConstructLocalVariable(const LocalVariableSignature &signature) {
    this->localVarMap_[nextVariableId_] = std::make_shared<LocalVariable>(nextVariableId_, signature);
    return this->localVarMap_[nextVariableId_++];
}

std::shared_ptr<LocalVariable>WeaveGraph::ConstructorArrayLocalVariable(const std::string &name, const MType &template_type) {
    LocalVariableSignature signature = {
        .name = name,
        .type = {
            .type = MValueTypes::ARRAY,
            .type_args = {template_type}
        }
    };

    return ConstructLocalVariable(signature);
}
std::shared_ptr<LocalVariable> WeaveGraph::ConstructorMapLocalVariable(const std::string &name, const MType &key_template_type, const MType &value_template_type) {
    LocalVariableSignature signature = {
        .name = name,
        .type = {
            .type = MValueTypes::ARRAY,
            .type_args = {key_template_type, value_template_type}
        }
    };
    return ConstructLocalVariable(signature);
}

std::shared_ptr<FunctionStatementNode> WeaveGraph::CreateFunctionStatementNode(
    const std::string &name, const std::vector<LocalVariable> &params, MType return_variable_type,
    const std::vector<LocalVariable> &local_vars) {

    const WeaveNodeId id = GenerateNodeId();
    auto result = std::make_shared<FunctionStatementNode>(id, name, params, return_variable_type, local_vars);
    this->function_statements.push_back(id);
    nodes_.emplace(id, result);
    return result;
}

std::shared_ptr<WeaveGraphNode> WeaveGraph::GetNode(const WeaveNodeId id) const {
    const auto it = nodes_.find(id);
    if (it == nodes_.end()) {
        return nullptr;
    }

    return it->second;
}

bool WeaveGraph::ContainsNode(const WeaveNodeId id) const noexcept {
    return nodes_.contains(id);
}

std::size_t WeaveGraph::GetNodeCount() const noexcept {
    return nodes_.size();
}

WeaveNodeId WeaveGraph::PeekNextNodeId() const noexcept {
    return nextNodeId_;
}

WeaveNodeId WeaveGraph::GenerateNodeId() noexcept {
    return nextNodeId_++;
}

} // namespace Mirage::EditorCore::WaveGraph
