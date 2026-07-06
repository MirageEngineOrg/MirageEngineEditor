#include "WaveGraph/Nodes/Statements/SetLocalVariableStatementNode.h"

namespace Mirage::EditorCore::WaveGraph {

WeaveGraphNodeKind SetLocalVariableStatementNode::GetKind() const noexcept {
    return WeaveGraphNodeKinds::SetLocalStatement;
}

} // namespace Mirage::EditorCore::WaveGraph
