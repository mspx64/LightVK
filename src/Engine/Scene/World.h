#pragma once

#include <functional>
#include "Engine/Scene/SceneGraph.h"
#include "Engine/Core/GlmConfig.h"
#include "Engine/Scene/Systems/Transform.h"
#include <entt/entt.hpp>

namespace Lgt {

// forward decals ----------------------------
namespace Gpu {
struct DrawList;
}

namespace System {
class Transform;
}

class Entity;
// -------------------------------------------

class World {
public:
    explicit World();
    ~World() = default;

    Entity CreateEntity(std::string name = "Entity");
    void   DestroyEntity(Entity entity);
    void   Update(float deltaTime);
    
    entt::registry&       Registry() { return m_Registry; }
    const entt::registry& Registry() const { return m_Registry; }
    SceneGraph&           Graph() { return graph_; }
    Gpu::DrawList         DrawList();

private:
    System::Transform transform_sys;
    entt::registry    m_Registry;
    SceneGraph        graph_;
};

} // namespace Lgt