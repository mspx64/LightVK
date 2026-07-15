#include "Editor.h"
#include <imgui.h>

namespace Lgt {

namespace Editor {

void Editor::Init(World* world) {
    context_.world = world;
    panel_hirearchy_.Init(&context_);
}

void Editor::Update() {
    // Editor UI
    panel_hirearchy_.Draw();
    ImGui::ShowDemoWindow();
}

void Editor::Shutdown() {}
} // namespace Editor

} // namespace Lgt
