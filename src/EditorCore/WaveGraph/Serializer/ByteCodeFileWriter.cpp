#include "WaveGraph/Serializer/ByteCodeFileWriter.h"

ByteCodeFileWriter::ByteCodeFileWriter(const fs::path& path) {
    if (!fs::exists(path.parent_path())) {
        bool result = fs::create_directories(path.parent_path());
        if (!result) {
            throw std::runtime_error("Couldnt create path for serializing bytecode");
        }
    }

    file_ = std::ofstream(path, std::ios::binary);

    if (!file_) {
        throw std::runtime_error("Couldnt open bytecode file");
    }

    uint64_t zero = 0;
    file_.write(reinterpret_cast<const char*>(&zero), sizeof(zero));
}

void ByteCodeFileWriter::WriteHeader(uint32_t first, uint32_t second)
{
    auto currentPos = file_.tellp();

    file_.seekp(0, std::ios::beg);

    file_.write(reinterpret_cast<const char*>(&first), sizeof(first));
    file_.write(reinterpret_cast<const char*>(&second), sizeof(second));

    file_.seekp(currentPos);
}

ByteCodeFileWriter::~ByteCodeFileWriter() {
    if (file_.is_open()) {
        file_.close();
    }
}

void ByteCodeFileWriter::SerializeBytearray(
    const std::vector<char>& bytecode)
{
    // SerializeInt<uint32_t>(
    //     static_cast<uint32_t>(bytecode.size())
    // );

    if (!bytecode.empty())
    {
        file_.write(
            bytecode.data(),
            static_cast<std::streamsize>(bytecode.size())
        );
    }
}


void ByteCodeFileWriter::SerializeStringTable(
    const std::vector<std::string>& strtab)
{
    SerializeInt<uint32_t>(
        static_cast<uint32_t>(strtab.size())
    );

    for (const auto& str : strtab)
    {
        if (!str.empty())
        {
            file_.write(
                str.data(),
                static_cast<std::streamsize>(str.size())
            );
        }

        char zero = '\0';
        file_.write(&zero, 1);
    }
}

void ByteCodeFileWriter::SerializeNameIDs(
    const std::vector<MNameID>& names)
{
    SerializeInt<uint32_t>(
        static_cast<uint32_t>(names.size())
    );

    for (MNameID id : names)
    {
        SerializeInt(id);
    }
}

void ByteCodeFileWriter::WriteRelocs(
    const std::unordered_map<uint32_t, uint32_t>& relocs)
{
    uint32_t count = static_cast<uint32_t>(relocs.size());

    file_.write(
        reinterpret_cast<const char*>(&count),
        sizeof(count));

    for (const auto& [key, value] : relocs)
    {
        file_.write(
            reinterpret_cast<const char*>(&key),
            sizeof(key));

        file_.write(
            reinterpret_cast<const char*>(&value),
            sizeof(value));
    }
}

uint32_t ByteCodeFileWriter::GetCurrentFileOffset() {
    uint32_t offset = static_cast<uint32_t>(file_.tellp());
    return offset;
}
