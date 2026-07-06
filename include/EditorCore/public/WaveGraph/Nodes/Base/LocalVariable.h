#pragma once
#include <cstdint>
#include <string>
#include "Meta/MValueKind.h"

using LocalVariableId = uint32_t;

struct LocalVariableSignature {
    std::string name;
    MType type;
};

struct LocalVariable {
    LocalVariableId id;
    LocalVariableSignature signature;
};