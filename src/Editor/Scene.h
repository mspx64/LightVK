#pragma once
#include <filesystem>
#include <functional>
#include <glm/glm.hpp>
#include <entt/entt.hpp>

#include "Assets.h"
#include "Gpu/Renderer.h"

namespace Lgt::Editor {
/*the plan -> ? editor loads the gltf and all the assets and builds a map and then the game loads that map at
runtime that way we don't have to worry about the editor to runtime transition  */

class Scene {
private:
    Gpu::DrawList  drawList_;
    entt::registry registry_;

public:
    void                 updateDrawList();
    const Gpu::DrawList& drawList() const { return drawList_; }

    void addModle(const Assets::Model&, bool dynamic = true, glm::mat4 transform = glm::mat4(1.0f));
};

} // namespace  Lgt::Editor
