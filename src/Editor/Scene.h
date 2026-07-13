#pragma once
#include <filesystem>
#include <functional>
#include <glm/glm.hpp>
#include <entt/entt.hpp>

#include "Assets.h"
#include <Core/Gpu/Renderer.h>

namespace Lgt::Editor {
/*the plan -> ? editor loads the gltf and all the assets and builds a map and then the game loads that map at
runtime that way we don't have to worry about the editor to runtime transition  */

class Scene {
private:
    Gpu::DrawList  drawList_;
    entt::registry registry_;
public:
    void           updateDrawList();
    Gpu::DrawList* drawList() { return &drawList_; }
};

} // namespace  Lgt::Editor
