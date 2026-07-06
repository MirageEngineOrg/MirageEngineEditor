#pragma once
#include "Nodes/WeaveExpressionNode.hpp"

class NodeKindHelper {
public:
    static bool is_kind_comparable(WeaveValueKind kind);
    static bool is_kind_numeric(WeaveValueKind kind);
};