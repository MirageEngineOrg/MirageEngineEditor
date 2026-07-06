#include "WaveGraph/NodeKindHelper.h"

bool NodeKindHelper::is_kind_comparable(WeaveValueKind kind) {
    return is_kind_numeric(kind);
}
bool NodeKindHelper::is_kind_numeric(WeaveValueKind kind) {
    return kind == WeaveValueKindValues::Double || kind == WeaveValueKindValues::Float || kind == WeaveValueKindValues::Integer;
}