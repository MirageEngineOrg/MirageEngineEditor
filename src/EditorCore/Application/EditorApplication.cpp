#include "Application/EditorApplication.hpp"

#include "Application/EditorApplicationInternal.hpp"

#include <utility>

namespace Mirage::EditorCore {

EditorApplication::EditorApplication(EditorApplicationConfig config)
    : config_(std::move(config)) {
}

const EditorApplicationConfig& EditorApplication::GetConfig() const noexcept {
    return config_;
}

std::string_view EditorApplication::GetLinkedEngineName() noexcept {
    return kLinkedEngineName;
}

} // namespace Mirage::EditorCore
