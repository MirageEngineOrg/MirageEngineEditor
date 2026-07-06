#pragma once

#include "WaveGraph/WeaveGraphTypes.hpp"

#include <cstdint>

namespace Mirage::EditorCore::WaveGraph {

using WeaveGraphNodeKind = std::uint64_t;

namespace WeaveGraphNodeKinds {

constexpr WeaveGraphNodeKind Unknown = 0;

constexpr WeaveGraphNodeKind ExpressionKindRangeBegin = 1;
constexpr WeaveGraphNodeKind Expression = 1;

constexpr WeaveGraphNodeKind ConstantKindRangeBegin = 16;
constexpr WeaveGraphNodeKind Constant = 16;
constexpr WeaveGraphNodeKind IntegerConstant = 17;
constexpr WeaveGraphNodeKind StringConstant = 18;
constexpr WeaveGraphNodeKind DoubleConstant = 19;
constexpr WeaveGraphNodeKind FloatConstant = 20;
constexpr WeaveGraphNodeKind ConstantKindRangeEnd = 31;

constexpr WeaveGraphNodeKind BinaryOperationKindRangeBegin = 32;
constexpr WeaveGraphNodeKind BinaryOperation = 32;
constexpr WeaveGraphNodeKind SumOperation = 33;
constexpr WeaveGraphNodeKind SubOperation = 34;
constexpr WeaveGraphNodeKind MulOperation = 35;
constexpr WeaveGraphNodeKind DivideOperation = 36;
constexpr WeaveGraphNodeKind ModOperation = 37;
constexpr WeaveGraphNodeKind EqualOperation = 38;
constexpr WeaveGraphNodeKind NotEqualOperation = 39;
constexpr WeaveGraphNodeKind LessOperation = 40;
constexpr WeaveGraphNodeKind LessOrEqualOperation = 41;
constexpr WeaveGraphNodeKind GreaterOperation = 42;
constexpr WeaveGraphNodeKind GreaterOrEqualOperation = 43;
constexpr WeaveGraphNodeKind LogicalAndOperation = 44;
constexpr WeaveGraphNodeKind LogicalOrOperation = 45;
constexpr WeaveGraphNodeKind BitwiseAndOperation = 46;
constexpr WeaveGraphNodeKind BitwiseOrOperation = 47;
constexpr WeaveGraphNodeKind BinaryOperationKindRangeEnd = 63;

constexpr WeaveGraphNodeKind GetLocalVariableExpression = 64;
constexpr WeaveGraphNodeKind GetClassVariableExpression = 65;
constexpr WeaveGraphNodeKind FunctionCallExpression = 66;

constexpr WeaveGraphNodeKind ExpressionKindRangeEnd = 255;


constexpr WeaveGraphNodeKind StatementKindRangeBegin = 256;
constexpr WeaveGraphNodeKind SetLocalStatement = 257;
constexpr WeaveGraphNodeKind FunctionStatement = 257;
constexpr WeaveGraphNodeKind ExpressionStatement = 258;
constexpr WeaveGraphNodeKind StatementKindRangeEnd = 512;

[[nodiscard]] constexpr bool IsExpressionKind(const WeaveGraphNodeKind kind) noexcept {
    return kind >= ExpressionKindRangeBegin && kind <= ExpressionKindRangeEnd;
}

[[nodiscard]] constexpr bool IsConstantKind(const WeaveGraphNodeKind kind) noexcept {
    return kind >= ConstantKindRangeBegin && kind <= ConstantKindRangeEnd;
}

[[nodiscard]] constexpr bool IsBinaryOperationKind(const WeaveGraphNodeKind kind) noexcept {
    return kind >= BinaryOperationKindRangeBegin && kind <= BinaryOperationKindRangeEnd;
}

} // namespace WeaveGraphNodeKinds

class WeaveGraphNode {
public:
    virtual ~WeaveGraphNode() = default;

    WeaveGraphNode(const WeaveGraphNode&) = delete;
    WeaveGraphNode& operator=(const WeaveGraphNode&) = delete;
    WeaveGraphNode(WeaveGraphNode&&) = delete;
    WeaveGraphNode& operator=(WeaveGraphNode&&) = delete;

    [[nodiscard]] WeaveNodeId GetId() const noexcept;
    [[nodiscard]] virtual WeaveGraphNodeKind GetKind() const noexcept = 0;
protected:
    explicit WeaveGraphNode(WeaveNodeId id) noexcept;

private:
  WeaveNodeId id_{0};
};

} // namespace Mirage::EditorCore::WaveGraph
