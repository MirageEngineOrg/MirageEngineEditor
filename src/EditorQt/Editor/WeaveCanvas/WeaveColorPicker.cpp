#include "Editor/WeaveCanvas/WeaveColorPicker.h"

using Mirage::EditorCore::WaveGraph::WeaveGraphNodeKind;
namespace WeaveGraphNodeKinds = Mirage::EditorCore::WaveGraph::WeaveGraphNodeKinds;

QColor WeaveColorPicker::GetPinColor(const WeaveGraphNodeKind kind) noexcept {
    if (kind == WeaveGraphNodeKinds::IntegerConstant) {
        return QColor(94, 168, 255);
    }

    if (kind == WeaveGraphNodeKinds::StringConstant) {
        return QColor(110, 214, 160);
    }

    if (kind == WeaveGraphNodeKinds::DoubleConstant) {
        return QColor(186, 132, 255);
    }

    if (kind == WeaveGraphNodeKinds::FloatConstant) {
        return QColor(255, 132, 189);
    }

    if (WeaveGraphNodeKinds::IsBinaryOperationKind(kind)) {
        return QColor(255, 168, 76);
    }

    return QColor(138, 149, 168);
}

QColor WeaveColorPicker::GetNodeColor(const WeaveGraphNodeKind kind) noexcept {
    if (kind == WeaveGraphNodeKinds::IntegerConstant) {
        return QColor(52, 97, 150);
    }

    if (kind == WeaveGraphNodeKinds::StringConstant) {
        return QColor(46, 120, 89);
    }

    if (kind == WeaveGraphNodeKinds::DoubleConstant) {
        return QColor(102, 74, 147);
    }

    if (kind == WeaveGraphNodeKinds::FloatConstant) {
        return QColor(150, 70, 110);
    }

    if (WeaveGraphNodeKinds::IsBinaryOperationKind(kind)) {
        return QColor(140, 86, 38);
    }

    return QColor(78, 95, 118);
}
