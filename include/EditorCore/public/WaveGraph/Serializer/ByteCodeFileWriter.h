#pragma once
#include <Name/MName.h>
#include <filesystem>
#include <fstream>
#include <iosfwd>
#include <vector>
#include <unordered_map>
namespace fs = std::filesystem;

class ByteCodeFileWriter {
public:
    ByteCodeFileWriter(const fs::path &path);
    ~ByteCodeFileWriter();
    ByteCodeFileWriter(const ByteCodeFileWriter& other) = delete;
    ByteCodeFileWriter(ByteCodeFileWriter&& other) = delete;
    ByteCodeFileWriter operator=(const ByteCodeFileWriter& other) = delete;
    ByteCodeFileWriter operator=(ByteCodeFileWriter&& other) = delete;

    void WriteHeader(uint32_t first, uint32_t second);
    void SerializeBytearray(const std::vector<char>& bytecode);
    void SerializeStringTable(const std::vector<std::string>& strtab);
    void SerializeNameIDs(const std::vector<MNameID>& names);
    void WriteRelocs(const std::unordered_map<uint32_t, uint32_t>& relocs);
    uint32_t GetCurrentFileOffset();

    template <typename T>
    void SerializeInt(T value);

private:
    std::ofstream file_;
};

template<typename T>
void ByteCodeFileWriter::SerializeInt(T value)
{
    static_assert(std::is_integral_v<T>,
        "SerializeInt supports only integral types");

    file_.write(
        reinterpret_cast<const char*>(&value),
        sizeof(T)
    );
}
