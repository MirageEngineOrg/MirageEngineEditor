#include "WaveGraph/Nodes/Statements/FunctionStatementNode.h"
namespace  Mirage::EditorCore::WaveGraph {

FunctionStatementNode::FunctionStatementNode(WeaveNodeId nodeId,
        const std::string& name,
        const std::vector<LocalVariable>& params,
        MType return_variable_type,
        const std::vector<LocalVariable>& local_vars) : StatementNode(nodeId) {

    this->name = name;
    this->signature_.returnType = return_variable_type;
    this->local_variables.resize(params.size() + 2 + local_vars.size());

    this->local_variables[0] = LocalVariable(0, {"this", {.type = MValueTypes::OBJECT_PTR} });
    this->local_variables[1] = LocalVariable(1, {"RETURN_VALUE", return_variable_type});
    this->glob_to_local_id[0] = 0;
    this->glob_to_local_id[1] = 1;

    uint32_t id_counter = 2;
    for (uint32_t i = 0; i < params.size(); ++i) {
        this->signature_.params.push_back(params[i].signature.type);
        this->local_variables[id_counter] = params[i];
        this->glob_to_local_id[params[i].id] = id_counter++;
    }

    for (uint32_t i = 0; i < local_vars.size(); i++) {
        this->glob_to_local_id[local_vars[i].id] = id_counter;
        this->local_variables[id_counter++] = local_vars[i];
    }
}

void FunctionStatementNode::AddLocalVariable(const LocalVariable &local_variable) {
    this->glob_to_local_id[local_variable.id] = this->local_variables.size();
    this->local_variables.push_back(local_variable);
}

void FunctionStatementNode::RemoveLocalVariable(LocalVariableId id_to_remove) {
    if (!this->glob_to_local_id.contains(id_to_remove)) {
        return;
    }
    uint32_t local_index = this->glob_to_local_id[id_to_remove];
    for (auto& id_to_loc : this->glob_to_local_id) {
        if (id_to_loc.second > local_index) {
            id_to_loc.second--;
        }
    }
    this->local_variables.erase(this->local_variables.begin() + local_index);
}

std::string FunctionStatementNode::GetName() const {
    return this->name;
}

void FunctionStatementNode::Rename(const std::string &new_name) {
    this->name = new_name;
}

void FunctionStatementNode::AddParam(const LocalVariableSignature &param_signature) {

}

void FunctionStatementNode::RemoveParam(int paramIdx) {

}

void FunctionStatementNode::SetReturnType(MValueType return_type) {

}
MFunctionSignature FunctionStatementNode::GetSignature() const {
    return signature_;
}
const std::unordered_map<LocalVariableId, uint32_t> & FunctionStatementNode::GetGlobToLocalIds() const {
    return this->glob_to_local_id;
}
const std::vector<LocalVariable> &FunctionStatementNode::GetLocalVariables() const {
    return this->local_variables;
}

WeaveGraphNodeKind FunctionStatementNode::GetKind() const noexcept {
    return WeaveGraphNodeKinds::FunctionStatement;
}


} // namespace Mirage::EditorCore::WaveGraph
