#pragma once
#include <filesystem>
#include <memory>

#include "WaveGraph/Serializer/WeaveGraphBytecodeCollector.h"
#include "WaveGraph/Serializer/ByteCodeFileWriter.h"

namespace fs = std::filesystem;

namespace Mirage::EditorCore::WaveGraph {

class WeaveGraph;
class WeaveGraphSerializer {
public:
    static WeaveGraphSerializer& GetInstance();
    void Serialize(const fs::path& path, std::shared_ptr<WeaveGraph> graph);
private:
    void SerializeFunctions(GraphBytecode* init_function_bytecode, std::unordered_map<uint32_t, uint32_t>& relocs, std::shared_ptr<WeaveGraph> graph,ByteCodeFileWriter& writer);
    void SerializeFunctionMeta(const FunctionMetaInfo &function_meta, ByteCodeFileWriter& writer);

    WeaveGraphSerializer() = default;
};



} // namespace Mirage::EditorCore::WaveGraph
