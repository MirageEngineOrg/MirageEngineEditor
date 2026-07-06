#pragma once
#include "StatementNode.h"
#include "WaveGraph/Nodes/Base/LocalVariable.h"
#include <unordered_map>
#include <vector>
#include "Meta/Field/MFunction.h"

namespace  Mirage::EditorCore::WaveGraph {

class FunctionStatementNode : public StatementNode {
public:
    explicit FunctionStatementNode(WeaveNodeId nodeId,
        const std::string& name,
        const std::vector<LocalVariable>& params,
        MType return_variable_type,
        const std::vector<LocalVariable>& local_vars);
    [[nodiscard]] WeaveGraphNodeKind GetKind() const noexcept override;

    void AddLocalVariable(const LocalVariable& local_variable);
    void RemoveLocalVariable(LocalVariableId id_to_remove);

    std::string GetName() const;
    void Rename(const std::string& new_name);

    void AddParam(const LocalVariableSignature& param_signature);
    void RemoveParam(int paramIdx);

    void SetReturnType(MValueType return_type);

    MFunctionSignature GetSignature() const;
    const std::unordered_map<LocalVariableId, uint32_t>& GetGlobToLocalIds() const;
    const std::vector<LocalVariable>& GetLocalVariables() const;


    std::vector<LocalVariable> local_variables;
private:
    std::string name;
    std::unordered_map<LocalVariableId, uint32_t> glob_to_local_id;

    MFunctionSignature signature_;
};

}