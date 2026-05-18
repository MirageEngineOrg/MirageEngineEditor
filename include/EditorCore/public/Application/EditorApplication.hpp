#pragma once

#include <string>
#include <string_view>

namespace Mirage::EditorCore {

struct EditorApplicationConfig {
    std::string projectName;
};

class EditorApplication {
public:
    explicit EditorApplication(EditorApplicationConfig config);

    [[nodiscard]] const EditorApplicationConfig& GetConfig() const noexcept;
    [[nodiscard]] static std::string_view GetLinkedEngineName() noexcept;

private:
    EditorApplicationConfig config_;
};

} // namespace Mirage::EditorCore
